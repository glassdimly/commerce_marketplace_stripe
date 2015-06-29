<?php 
/**
 * Menu callback. Saves user Stripe Access Token when redirected by successful Stripe signup page.
 * @see https://stripe.com/docs/connect/standalone-accounts
 */
function commerce_stripe_marketplace_connect() {
  $store_id = $_SESSION['commerce_marketplace_stripe_store'];
  try {
    if (empty($store_id)){
      throw new Exception(t('$_SESSION parameter store is unset. Our bad.'));
    }

    if (isset($_GET['error']) && $_GET['error'] == 'access_denied'){
      throw new Exception(t('Stripe Connect error. It appears you did not allow the application access to your account. If you haven\'t done so already, try again, but grant access this time. (@error: @description)', array('@error' => $_GET['error'], '@description' => $_GET['error_description'])));
    }
    
    if (isset($_GET['error'])){
      throw new Exception(t('Stripe Connect error: @error: @description', array('@error' => $_GET['error'], '@description' => $_GET['error_description'])));
    }

    if (empty($_GET['code'])){
      throw new Exception(t('Stripe Connect error: ' . 'Code from Stripe came back empty.'));
    }

    if ($_SESSION['commerce_marketplace_stripe_state'] != $_GET['state']) {
      throw new Exception(t('Stripe Connect error: The state we sent over to Stripe came back different.'));
    }

    //Get the rule settings from the Stripe payment method
    $settings = commerce_marketplace_stripe_get_payment_action_settings();
    $params = array(
      'client_secret' => $settings['secret_key'],
      'code' => $_GET['code'],
      'grant_type' => 'authorization_code'
    );

    $options = array(
      'method' => 'POST',
      'data' => http_build_query($params)
    );

    //This request takes the auth token and then requests a token from Stripe to seal the authentication.
    $result = drupal_http_request('https://connect.stripe.com/oauth/token', $options);
    $json = json_decode($result->data);

    if (isset($json->error)){
      //$json->error_description contains the secret Stripe key that was submitted. Strip it out zealously.
      $sanitized_error = preg_replace(array('#ca_.*#', '#pk_.*#', '#sk_.*#'), array('(ca key removed)', '(pk key removed)', '(sk key removed)'), $json->error_description);
      throw new Exception(t('Request for permanent token error: @error: @description', array('@error'=>$json->error, '@description'=>$sanitized_error)));
    }

    $store = commerce_store_load($store_id);
    if(empty($store)){
      throw new Exception(t('Store #@storeid didn\'t load', array('@store_id' => $store_id)));
    }

    // Check user access against store
    global $user;
    $access = commerce_store_access('update', $store, $user);
    if (!$access) {
      throw new Exception(t('User @name does not have access to store id #@store_id', array('@name'=>$user->name, '@store_id' => $store_id)));
    }

    //Save the entire response to the data object on the store.
    $store->data['payment_methods']['settings']['commerce_stripe'] = commerce_marketplace_stripe_get_payment_action_settings();

    //Application will be the Stripe account in the configured rule
    $application = commerce_marketplace_stripe_get_payment_action_settings();

    //Provide the default values from the configured rule.
    $store->data['payment_methods']['settings']['commerce_stripe'] = $application;

    $connected = get_object_vars($json);

    //Merge in the secret keys and private keys so they're used for the initial charge.
    $store->data['payment_methods']['settings']['commerce_stripe']['secret_key'] = $connected['access_token'];
    $store->data['payment_methods']['settings']['commerce_stripe']['public_key'] = $connected['stripe_publishable_key'];

    //Save the original rule and all the connected details as is. TODO store less? Perhaps.
    $store->data['payment_methods']['settings']['commerce_stripe']['application'] = $application;
    $store->data['payment_methods']['settings']['commerce_stripe']['connected'] = $connected;

    commerce_store_save($store);

    drupal_set_message('Your Stripe account has been connected. Administer it at <a href="https://stripe.com">Stripe.com</a>. Now that your Stripe account has been set to receive payments you must set it up to disperse payments to your bank <a href="https://dashboard.stripe.com/account/transfers">here</a>, if you have not already done so.');

  } catch (Exception $e){
    watchdog('commerce_marketplace_stripe', 'Stripe connect failed with: @message', array('@message' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_set_message('Stripe connect failed. Please contact and administrator with this message: ' . $e->getMessage(), 'error');
  } catch(EntityMetadataWrapperException $e){
    watchdog('commerce_marketplace_stripe', 'Store #@store_id failed to save: @message', array('@store_id' => $store->store_id, '@message' => $e->getMessage()), WATCHDOG_ERROR);
    drupal_set_message('Store #' . $store->store_id . ' failed to save. Please contact and administrator with this message: ' . $e->getMessage(), 'error');
  }

  global $base_url;
  drupal_goto($base_url . '/store/' . $store_id .'/payment-methods/commerce_stripe');
}

<?php

function commerce_marketplace_stripe_store_form($form, &$form_state, $store){
  $settings = commerce_marketplace_stripe_get_payment_action_settings();
  if (empty($settings) || empty($settings['client_id'])){
    $form['empty_settings']['#markup'] = t('The Stripe payment rule has not been configured. You, or an administrator, must configure it <a href="/admin/config/workflow/rules/reaction/manage/commerce_payment_commerce_stripe">here</a>.');
    return $form;
  }

  $form['#store'] = $store;

  $form['#stripe_settings'] = $settings;

  $connected = isset($store->data['payment_methods']['settings']['commerce_stripe']['connected']);

  //TODO listen for the deauthorized web hook https://stripe.com/docs/connect/standalone-accounts#revoked-access
  if ($connected){
    $form['already_connected']['#markup'] = t('<p><b>Your store is already connected.</b> If you are having trouble, revoke access to the FreshStop application on your Stripe account <a href="https://dashboard.stripe.com/account/applications">here</a> and re-connect below. Now that your Stripe account has been set to receive payments you must set it up to disperse payments to your bank <a href="https://dashboard.stripe.com/account/transfers">here</a>, if you have not already done so.</p>');
  }

  $form['use_conditions'] = array(
    '#type' => 'markup',
    '#markup' => check_markup($settings['use_conditions']['value'], $settings['use_conditions']['format']),
  );

  $form['submit'] = array(
    '#type' => 'image_button',
    '#src' => '/' . drupal_get_path('module', 'commerce_marketplace_stripe') . '/theme/connect-button.png',
    '#value' => 'Save',
  );

  return $form;
}
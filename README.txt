WHAT IS IT?
This Drupal module provides integration between the Commerce Marketplace module (https://github.com/maciejzgadzaj/commerce_marketplace) and Commerce Stripe (https://github.com/aviindub/commerce_stripe) with a smallish patch to Commerce Stripe (forked here: https://github.com/glassdimly/commerce_stripe).

Commerce Marketplace allows Drupal Commerce users to create and manage their own stores and then configure their own payment processors. Commerce Marketplace Stripe allows users to configure the Stripe payment method via Stripe Connect. Users connect their stores to their Stripe account and then payments are sent directly into their Stripe accounts. An application fee expressed as a percentage can be drawn from each charge into the sitewide Stripe account.

INSTRUCTIONS
Enable Commerce Stripe, enable it as a payment method, then visit /admin/config/workflow/rules/reaction/manage/commerce_payment_commerce_stripe to enter your keys and configure sitewide settings. Finally, connect your stores at and /admin/commerce/stores -> (My store) Payment Methods -> Stripe to connect a store.

Currently, only the use of the commerce_stripe auto-configured rule element with the machine name of commerce_payment_commerce_stripe is supported. Rules that enable the action "Enable payment method: Stripe" which are not that rule will be ignored.

If support of multiple store-wide payment configurations is desired, open an issue in the module queue.
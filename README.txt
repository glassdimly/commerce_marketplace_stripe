Enable Commerce Stripe, enable it as a payment method, then visit /admin/config/workflow/rules/reaction/manage/commerce_payment_commerce_stripe to enter your keys and configure sitewide settings. Finally, connect your stores at and /admin/commerce/stores -> (My store) Payment Methods -> Stripe to connect a store.

Currently, only the use of the commerce_stripe auto-configured rule element with the machine name of commerce_payment_commerce_stripe is supported. Rules that enable the action "Enable payment method: Stripe" which are not that rule will be ignored.

If support of multiple store-wide payment configurations is desired, open an issue in the module queue.
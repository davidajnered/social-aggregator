<?php
require_once('SocialAggregatorPlugin.inc');
class SocialAggregatorTwitter extends SocialAggregatorPlugin {

  /**
   * User details
   */
  public function config() {
    $config = array(
      'plugin name' => 'Twitter For SA',
      'user config' => array(
        'username' => array(
          'required' => TRUE,
          'type' => 'text',
        ),
        'password' => array(
          'required' => TRUE,
          'type' => 'password',
        ),
      )
    );
    $this->savePluginConfig($config);
  }

  /**
   * Implementation of hook_data
   * Get the data from service api
   */
  public function data() {
    $params = func_get_args();
    error_log(var_export($params, TRUE));
  }
}
<?php
abstract class SocialAggregatorPlugin {

  /**
   * Define the abstract methods
   */
   abstract public function config();
   abstract public function data();

  /**
   * Add the plugin class name to the options array
   */
  public static function savePluginConfig($config) {
    $stored_config_array = get_option('SocialAggregatorPlugins');
    if($stored_config_array == NULL) {
      $stored_config_array = array();
    }
    $stored_config_array[$config['plugin name']] = $config;
    update_option('social_aggregator_plugins', $stored_config_array);
  }
  
  public function dostuff() {
    
  } 
}
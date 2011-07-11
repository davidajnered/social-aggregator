<?php
abstract class SocialAggregatorPlugin {

  private $plugin_name;

  /**
   * Define the abstract methods
   */
   abstract public function config();
   abstract public function data();
   abstract public function render();

  /**
   * Add the plugin class name to the options array
   */
  public function savePluginConfig($config) {
    $stored_config_array = get_option('social_aggregator_plugins');
    if(!$stored_config_array) {
      $stored_config_array = array();
    }

    // Store plugin name separatly for easy access
    $this->plugin_name = $config['plugin name'];

    // add machine name for plugin and for all the fields
    $config['machine name'] = $this->getMachineName();
    foreach($config['user config'] as $name => $field) {
      $config['user config'][$name]['machine name'] = $this->convertToMachineName($name);
    }

    $stored_config_array[$config['plugin name']] = $config;
    update_option('social_aggregator_plugins', $stored_config_array);
  }

  // Add method that's called before the get data method to
  // check if we need to fetch new data or just use old

  /**
   * Execute cURL.
   * Can be overridden in child class if this method is insufficient
   */
  public function doCurl($url) {
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_POST, 0);
    $result = curl_exec($ch);
    curl_close($ch);
    return $result;
  }

  public function getName() {
    return $this->plugin_name;
  }

  public function getMachineName() {
    return strtolower(str_replace(' ', '_', $this->plugin_name));
  }
  
  private function convertToMachineName($name) {
    return strtolower(str_replace(' ', '_', $name));
  }
}
<?php
class SocialAggregatorPlugin {

  private $plugin;
  
  public function init($plugin) {
    error_log(var_export($plugin, TRUE));
    $this->plugin = new $plugin;
    return $this->plugin;
  }

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
    $config['machine name'] = self::getMachineName();
    foreach($config['user config'] as $name => $field) {
      $config['user config'][$name]['machine name'] = self::convertToMachineName($name);
    }

    $stored_config_array[$config['plugin name']] = $config;
    update_option('social_aggregator_plugins', $stored_config_array);
  }

  // Add method that's called before the get data method to
  // check if we need to fetch new data or just use old
  public function data() {
    $params = func_get_args();
    return $this->plugin->data($params);
  }
  
  public function config() {
    $config = $this->plugin->config();
    self::savePluginConfig($config);
  }
  
  public function render($content) {
    $this->plugin->render($content);
  }

  public function timestamp($data) {
    return $this->plugin->timestamp($data);
  }

  public function getName() {
    return $this->plugin_name;
  }

  public function getMachineName() {
    return strtolower(str_replace(' ', '_', $this->plugin_name));
  }
  
  public function getClassName() {
    return get_class($this->plugin);
  }
  
  private function convertToMachineName($name) {
    return strtolower(str_replace(' ', '_', $name));
  }
}
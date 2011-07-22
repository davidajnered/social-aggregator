<?php
/**
 * SocialAggregatorPlugin is a wrapper class for plugins.
 * It's purpose is to hide logic from developers looking to extending this plugin
 * One important function for this wrapper is to add hooks that enables front end designers
 * to override the render method with a custom function in function.php
 */

abstract class SocialAggregatorPlugin {

  private $values;

  abstract function config();
  abstract function data($params);
  abstract function timestamp($data);
  abstract function render($data);

  /**
   * Constructor
   * @param $plugin class name of plugin to be created
   */
  public function init() {
    // Store values in plugin
    $this->values = get_option('social_aggregator_' . self::getMachineName() . '_values', FALSE);
  }

  /**
   * Pre-plugin method.
   * Get the configuration array from the plugin. Called when setting up the admin form.
   * We add the active field to the plugin. This is needed for all plugins.
   * If we need to add more settings for all the plugins we do it here.
   * @return array defining configuration needed for the plugin
   */
  public function _config() {
    $config = $this->config();
    $config[] = array(
      'title'   => 'Active',
      'name'    => 'plugin-active',
      'type'    => 'checkbox',
      'value'   => array(
        array(
          'name' => 'active',
          'title' => 'Active',
        )
      )
    );
    return $config;
  }

  /**
   * Pre-plugin method.
   * Sets up the parameters defined in the plugins config method.
   * The values are given by the user and are stored in the plugins option array.
   * @param $params values stored in the database
   * @return the data retrieved from the plugin
   */
  public function _data($params = NULL) {
    $params = func_get_args();
    unset($params[0]['plugin-active']);
    return $this->data($params[0]);
  }

  /**
   * Pre-plugin method
   * Returns a unix timestamp for the post
   * This method have to be implemented by every plugin
   * since the framework doesn't know the structure of the data
   * @param $data the data
   * @return a unix timestamp formatted date
   */
  public function _timestamp($data) {
    return $this->timestamp($data);
  }

  /**
   * Pre-plugin method
   * Check if there's a function (should be in function.php but you never know) that overrides the plugin render method.
   * If there is one, we call it.
   * @param $data the data
   */
  public function _render($data) {
    $func = strtolower(self::getMachineName()) . '_render';
    if(function_exists($func)) {
      call_user_func($func, $data);
    }
    else {
      $this->render($data);
    }
  }

  // HELPER METHODS
  
  /**
   * Does a basic cURL request
   */
   function doCurl($url) {
     $ch = curl_init($url);
     curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
     curl_setopt($ch, CURLOPT_POST, 0);
     $result = curl_exec($ch);
     curl_close($ch);
     return $result;
   }

  /**
   * Get the class name of the plugin object
   * @return class name
   */
  public function getClassName() {
    return get_class($this);
  }

  /**
   * Return plugin name defined by the plugin author
   * @return plugin name
   */
  public function getPluginName() {
    return $this->pluginName;
  }

  /**
   * Get the plugin name as machine name with no spaces and uppercase characters
   * @param @name the name to be converted
   * @return the machine name
   */
  public function getMachineName($name = NULL) {
    if($name != NULL) {
      return strtolower(str_replace(' ', '_', $name));
    }
    return strtolower(str_replace(' ', '_', $this->pluginName));
  }

  /**
   * Get all the stored values for the plugin
   * @return the values
   */
  public function getValues() {
    return $this->values;
  }

  /**
   * Get a specific value for the plugin
   * @param $name the name of the value
   * @return the value
   */
  public function getValue($name = NULL) {
    if($name != NULL && isset($this->values[$name])) {
      return $this->values[$name];
    }
    return FALSE;
  }

  /**
   * Store new values for the plugin
   * @param $values the new values
   */
  public function storeValues($values) {
    update_option('social_aggregator_' . self::getMachineName() . '_values', $values);
  }

  /**
   * Check is this plugin is active.
   * @return TRUE if active, else FALSE
   */
  public function isActive() {
    if(isset($this->values['plugin-active'])) {
      return TRUE;
    }
    return FALSE;
  }
}
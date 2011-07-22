<?php
/**
 * ToDo: Remove javascript redirect to avoid admin form resubmission
 */
class SocialAggregator {

  // Variables
  private $pluginClasses;
  private $pluginObjects;
  private $admin;

  /**
   * Singleton construct method
   */
  public function SocialAggregator($admin) {
    $this->admin = $admin;
    self::scanPluginDir();
    self::initPlugins();
    self::fetchData();
  }

  /**
   * This is a setup function
   * We need to get data from the plugin like specifications for the API call to the service
   * This is stored in an the option array so we can display the correct fields on the settings page
   */
  public function initPlugins() {
    foreach($this->pluginClasses as $plugin) {
      // Create and instanciate all the present plugins
      $object = new $plugin();
      $object->init();
      $this->pluginObjects[$object->getClassName()] = $object;
    }
    $this->admin->setPlugins($this->pluginObjects);
    // Since the form redirects We check if settings form is submitted early.
    // This way we don't have to load data twice due to the redirect
    $this->admin->submit();
  }

  /**
   * Get the data from plugins api implementation
   */
  public function fetchData() {
    // Add a variable that decide how often we collect new data. We don't need to do it on every request
    foreach($this->pluginObjects as $plugin) {
      if(self::isPluginActive($plugin)) {
        $values = get_option('social_aggregator_' . $plugin->getMachineName() . '_values', array());
        $data = $plugin->_data($values);
        if(is_array($data)) {
          // Loop through all the posts and write them to the database
          foreach($data as $post) {
            self::dbWrite($plugin, $post);
          }
        }
      } else {
        self::dbDelete($plugin);
      }
    }
  }

  /**
   * Write plugin data to database
   * @param $plugin The plugin object
   * @param $data The post data to be inserted in the database
   * @return If query returns data we return the result object, else false
   */
  public function dbWrite($plugin, $post) {
    global $wpdb;
    $plugin_name = $plugin->getClassName();
    $table = $wpdb->prefix . "social_aggregator";
    $timestamp = $plugin->timestamp($post);
    $id = md5($plugin_name . $timestamp);
    $post = mysql_real_escape_string(serialize($post));
    $query = "INSERT IGNORE INTO " . $table . " (id, plugin, date, content) VALUES ('$id', '$plugin_name', $timestamp, '$post')";
    if ($wpdb->query($query) === FALSE) {
      return FALSE;
    }
    else {
      return $wpdb->get_results($query);
    }
  }

  /**
   * Delete all rows for a specific plugin
   * @param $plugin The plugin object
   */
  public function dbDelete($plugin) {
    global $wpdb;
    $table = $wpdb->prefix . "social_aggregator";
    $plugin_name = $plugin->getClassName();
    $query = "DELETE FROM " . $table . " WHERE plugin = '$plugin_name';";
    $wpdb->query($query);
  }

  /**
   * Get data from database and render to view
   * @return return false if render fails
   */
  public function data() {
    // This one needs improvement. Looks and feels like shit
    global $wpdb;
    $table = $wpdb->prefix . "social_aggregator";

    $loop = 0;
    foreach(self::getActivePlugins() as $plugin) {
      if($loop == 0) {
         $where = ' WHERE';
      }
      else {
        $where .= ' OR';
      }
      $where .= " plugin = '" . $plugin['class'] . "'";
      $loop++;
    }
    $query = "SELECT * FROM " . $table . $where . " ORDER BY date DESC LIMIT 0,30";
    if ($wpdb->query($query) === FALSE) {
      return FALSE;
    }
    else {
      $results = $wpdb->get_results($query);
      foreach($results as $result) {
        $content = unserialize($result->content); 
        // load the correct plugin object and call render with $content
        $pluginObject = $this->pluginObjects[$result->plugin];
        if(is_object($pluginObject)) {
          $pluginObject->render($content);
        }
      }
    }
  }

  /**
   * Scan through the plugin folder for available plugins
   */
  private function scanPluginDir() {
    $dir = dirname(__FILE__) . '/plugins';
    self::verifyDirectory($dir);
  }

  /**
   * Recursive method that search for plugins
   * @param $dir the directory to scan
   */
  private function verifyDirectory($dir) {
    static $subdir;
    $ignore = array('.', '..', 'SocialAggregatorPlugin.inc');
    if(is_dir($dir)) {
      if ($handle = opendir($dir)) {
        while (false !== ($file = readdir($handle))) {
          if(!in_array($file, $ignore)) {
            $file_path = $dir . '/' . $file;
            if(is_dir($file_path) && !$this->subdir) {
              $subdir = TRUE;
              self::verifyDirectory($file_path);
              break;
            } else {
              $result = self::verifyFile($file, $file_path);
              if($result && $subdir) {
                break;
              }
            }
          }
        }
      }
    }
  }

 /**
  * Validate file
  * @return true if file is a valid plugin, false for everything else
  * @param $file the file name
  * @param $file_path the file path
  */
  private function verifyFile($file, $file_path) {
    if(strpos($file, 'SocialAggregator') === 0) {
      require_once($file_path);
      $this->pluginClasses[] = str_replace('.inc', '', $file);
      // Load stored values and set the present once as active, the disabled once as passiv (1's and 0's)
      return TRUE;
    }
  }

  private function getPluginFromName($name) {
    foreach($this->pluginObjects as $plugin) {
      if($plugin->getMachineName() == $name || $plugin->getPluginName() == $name) {
        return $plugin;
      }
    }
    return FALSE;
  }
  
  private function getActivePlugins() {
    $active = array();
    foreach($this->pluginObjects as $plugin) {
      if($plugin->isActive()) {
        $active_plugin = array(
          'name' => $plugin->getPluginName(),
          'class' => $plugin->getClassName(),
        );
        $active[$plugin->getMachineName()] = $active_plugin;
      }
      else {
        unset($active[$plugin->getMachineName()]);
      }
    }
    return $active;
  }
  
  private function isPluginActive($plugin) {
    $active_plugins = self::getActivePlugins();
    if(isset($active_plugins[$plugin->getMachineName()])) {
      return TRUE;
    }
    return FALSE;
  }

} // End of class

/**
 * Autoload classes in the plugin folder
 */
function __autoload($class) {
  $file = dirname(__FILE__) . '/plugins/' . $class . '.inc';
  if(file_exists($file)) {
    require_once($file);
  }
}
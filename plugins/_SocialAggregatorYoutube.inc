<?php
class SocialAggregatorYoutube extends SocialAggregatorPlugin {

  /**
   * User details
   */
  public function config() {
    $config = array(
      'plugin name' => 'Youtube',
      'user config' => array(
        'username' => array(
          'required' => TRUE,
          'type' => 'text',
        ),
      ),
      'api return' => 'json',
    );
    $this->savePluginConfig($config);
  }

  /**
   * Implementation of hook_data
   * params[0] is an array containing all the stored data
   * Get the data from service api
   * most be returns in an structured array
   */
  public function data() {
    $url = 'https://gdata.youtube.com/feeds/api/users/davidajnered/uploads';
    $result = $this->doCurl($url);
    $result = unserialize($result);
    error_log(var_export($result, TRUE));
    return $this->sanitizeData($result);
  }

  // Specific
  private function buildURL($params) {
    $param_string = '';
    foreach($params as $name => $value) {
      $param_string .= $name . '=' . $value;
    }
    return $param_string;
  }

  /**
   * This method is specific for every plugin
   * Here we fetch the raw data and store it in an array defined by the plugin
   * We need to do this to get consistency through the plugin 
   * Fields:
   * content = main content
   * user = user name
   * date = unix timestamp
   */
  private function sanitizeData($data) {
    $posts = array();
    foreach($data as $raw) {
      $post['user']    = $raw->user->screen_name;
      $post['content'] = $raw->text;
      $post['date']    = strtotime($raw->created_at);
      $posts[] = $post;
    }
    return $posts;
  }
}
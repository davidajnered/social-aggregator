<?php
class SocialAggregatorTwitter extends SocialAggregatorPlugin {

  /**
   * User details
   */
  public function config() {
    $config = array(
      'plugin name' => 'Twitter For SA',
      'user config' => array(
        'screen name' => array(
          'required' => TRUE,
          'type' => 'text',
        ),
      )
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
    $base_url = 'http://api.twitter.com/1/statuses/user_timeline.json';
    $params = func_get_args();
    $url_params = $this->buildURL($params[0]);
    $url = $base_url . '?' . $url_params;
    $result = $this->doCurl($url);
    return json_decode($result);
  }
  
  $public function render() {
    // call_user_func_array($function, array());
    // print data
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
      $post = NULL;
      $post['user']    = $raw->user->screen_name;
      $post['content'] = $raw->text;
      $post['date']    = strtotime($raw->created_at);
      $posts[] = $post;
    }
    return $posts;
  }
}
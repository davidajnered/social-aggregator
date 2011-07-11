<?php
class SocialAggregatorVimeo extends SocialAggregatorPlugin {

  /**
   * User details
   */
  public function config() {
    $config = array(
      'plugin name' => 'Vimeo',
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
    $url = 'http://vimeo.com/api/v2/davidajnered/likes.php';
    $result = $this->doCurl($url);
    $result = unserialize($result);
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
    foreach($data as $key => $value) {
      $post['user_name'] = $value['user_name'];
      $post['user_url'] = $value['user_url'];
      $post['post_title'] = $value['title'];
      $post['post_url'] = $value['url'];
      $post['post_content'] = self::getImage($value['thumbnail_medium'], $value['url'], $key);
      $post['post_date'] = strtotime($value['upload_date']);
      $posts[] = $post;
    }
    return $posts;
  }
  
  private function getImage($url, $link = FALSE, $id = '') {
    $output = '';
    if($link) {
      $output .= '<a href="' . $link . '">';
    }
    $output .= '<img class="vimeo-image" id="vimeo-image-' . $id . '" src="' . $url . '">';
    if($link) {
      $output .= '</a>';
    }
    return $output;
  }
}
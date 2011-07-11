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
    $data = self::doCurl('http://vimeo.com/api/v2/davidajnered/likes.php');
    $data = unserialize($data);
    return $data;
  }

  public function timestamp($data) {
    return strtotime($data['liked_on']);
  }

  public function render() {
    // change!!!
    // print!!!
    $posts = array();
    foreach($data as $key => $value) {
      $post['user_name'] = $value['user_name'];
      $post['user_url'] = $value['user_url'];
      $post['post_title'] = $value['title'];
      $post['post_url'] = $value['url'];
      $post['post_content'] = $value['thumbnail_medium'];
      $post['post_date'] = strtotime($value['upload_date']);
      $posts[] = $post;
    }
    return $posts;
  }
}
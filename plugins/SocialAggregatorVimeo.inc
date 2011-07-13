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

  public function render($content) {
    // Output starts ?>

    <div>
      <h2>Vimeo: <?php print $content['title']; ?></h2>
      <img src="<?php print $content['thumbnail_medium']; ?>"><br/>
      <?php print substr($content['description'], 0, 100); ?>...
    </div>

  <?php
    // Output ends
  }
}
<?php
class SocialAggregatorTwitter extends SocialAggregatorPlugin {

  public $pluginName = 'Twitter';
  /**
   * User details
   */
  public function config() {
    $fields[] = array(
      'name' => 'screen_name',
      'title' => 'Username',
      'type' => 'text',
    );
    return $fields;
  }

  /**
   * Implementation of hook_data
   * params[0] is an array containing all the stored data
   * Get the data from service api
   * most be returns in an structured array
   */
  public function data($params) {
    $url = 'http://api.twitter.com/1/statuses/user_timeline.json?screen_name=' . $params['screen_name'];
    $ch = curl_init($url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_POST, 0);
    $result = curl_exec($ch);
    curl_close($ch);
    return json_decode($result);
  }

  public function timestamp($data) {
    return strtotime($data->created_at);
  }

  public function render($data) { ?>
      <div>
        <h2>Twitter</h2>
        <?php print $data->text; ?><br>
        <i><?php print $data->created_at; ?></i>
      </div>
    <?php
  }

}
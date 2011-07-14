<?php
class SocialAggregatorTwitter implements SocialAggregatorPluginInterface{

  /**
   * User details
   */
  public function config() {
    $config = array(
      'plugin name' => 'Twitter',
      'user config' => array(
        'screen name' => array(
          'required' => TRUE,
          'type' => 'text',
        ),
      )
    );
    return $config;
  }

  /**
   * Implementation of hook_data
   * params[0] is an array containing all the stored data
   * Get the data from service api
   * most be returns in an structured array
   */
  public function data($params) {
    $base_url = 'http://api.twitter.com/1/statuses/user_timeline.json';
    $url_params = $this->buildURL($params[0]);
    $url = $base_url . '?' . $url_params;
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

  public function render($content) { ?>
      <div>
        <h2>Twitter</h2>
        <?php print $content->text; ?><br>
        <i><?php print $content->created_at; ?></i>
      </div>
    <?php
  }

  // Specific
  private function buildURL($params) {
    if($params != NULL) {
      $param_string = '';
      foreach($params as $name => $value) {
        $param_string .= $name . '=' . $value;
      }
      return $param_string;
    }
    else {
      $e = sprintf('Social Aggregator: missing parameter for %s', $this->plugin_name);
      error_log(var_export($e, TRUE));
    }
  }

}
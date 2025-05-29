webview2_ready = false;

webview2_init(window_handle());

prompt_id = -1;

function webview_goto_start() {
    webview2_set_size(0,64,window_get_width(),window_get_height());
    webview2_set_visible(true);
    webview2_navigate_to_string("<!DOCTYPE html><html><head><title> WebView2.gml Usage Guide </title><style>body {font-family: sans-serif;}</style></head><body><p>Click in the black space above this \"browser\" and you can use the following controls<h1>Keys: </h1><ul><li>1. Navigate to specific page</li><li>2. Run custom JavaScript code on the webview</li><li>3. Get title of webpage in WebView</li></ul></body></html>");
}
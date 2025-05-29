if (!webview2_ready) {
    webview2_ready = webview2_is_ready();
    if (webview2_ready) {
        webview_goto_start();
    }
}

/// feather disable GM1017
if (webview2_ready) {
    if (keyboard_check_pressed(ord("1"))) {
        var url = get_string("URL to open","https://gamemaker.io");
        webview2_navigate(url);
    }
    if (keyboard_check_pressed(ord("2"))) {
        var code = get_string("Code to run","return 75;");
        prompt_id = webview2_execute_code(code);
        show_message(prompt_id);
    }
    if (keyboard_check_pressed(ord("3"))) {
        show_message(webview2_get_document_title());
    }
}

if (webview2_async_result_exists(prompt_id)) {
    var result = webview2_async_result_get(prompt_id);
    show_message($"You entered {result}");
    prompt_id = -1;
}
/*
 * utils file
 */
import toastr from 'toastr'
import 'toastr/build/toastr.css'

/*
 * Logger
 */
class Logger{
}

Logger.log = m => {
    console.log(m)
}


/*
 * Toast
 */
class Message{
    static show(m) {
        toastr.success(m)
    }
}


/*
 * Browser Utils
 */
class Browser{}

Browser.getParameterByName = (name, url) => {
    if (!url) url = window.location.href;
    name = name.replace(/[[\]]/g, "\\$&");
    var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
        results = regex.exec(url);
    if (!results) return null;
    if (!results[2]) return '';
    return decodeURIComponent(results[2].replace(/\+/g, " "));
}

export {Logger, Message, Browser}
import $ from 'jquery';
import 'bootstrap-material-design/dist/css/bootstrap-material-design.min.css';
import 'bootstrap-material-design';
import 'bootstrap-material-design-icons/css/material-icons.css'

import '../../assets/css/main.css';
import { Browser } from '../../utils/utils'
import Client from '../../utils/client'
import { AGORA_APP_ID } from '../../../static/agora.config';

$(() => {
    const appid = AGORA_APP_ID;
    let localAccount = Browser.getParameterByName("account");
    let client = new Client(appid);
    //by default call btn is disabled
    client.init(localAccount).then(() => {
        //once logged in, enable the call btn
        $(".startCallBtn").prop("disabled", false);
    });
});
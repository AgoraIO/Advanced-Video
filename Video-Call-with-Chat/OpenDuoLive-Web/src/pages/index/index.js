import $ from 'jquery';
import 'bootstrap/dist/css/bootstrap.min.css';
import 'bootstrap';

import '../../assets/css/main.css';

$(() => {
    $("#join-meeting").click(function (e) {
        //join btn clicked
        e.preventDefault();
        let account = $("#account-name").val();
        let channel = $("#channel-name").val();
        let role = $("input:checked").val();
        let valid = /^[0-9A-Za-z]+$/.test(account);
        if (account && channel && valid) {
            //account/channel has to be a non empty numeric value
            window.location.href = `meeting.html?account=${account}&channel=${channel}&role=${role}`;
        } else {
            !account && $("#account-name").removeClass("is-invalid").addClass("is-invalid");
            !channel && $("#channel-name").removeClass("is-invalid").addClass("is-invalid");
        }
    });
});
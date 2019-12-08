#ifndef WEBPAGE_H
#define WEBPAGE_H

const char webpageHTML[] = R"(
<!DOCTYPE html>
<html>

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>Configuración WiFi</title>
    <style>
        .login-page {
            width: 360px;
            padding: 8% 0 0;
            margin: auto;
        }

        .form {
            position: relative;
            z-index: 1;
            background: #FFFFFF;
            max-width: 360px;
            margin: 0 auto 100px;
            padding: 45px;
            text-align: center;
            box-shadow: 0 0 20px 0 rgba(0, 0, 0, 0.2), 0 5px 5px 0 rgba(0, 0, 0, 0.24);
        }

        .form input {
            font-family: sans-serif;
            outline: 0;
            background: #f2f2f2;
            width: 100%;
            border: 0;
            margin: 0 0 15px;
            padding: 15px;
            box-sizing: border-box;
            font-size: 14px;
        }

        .form button {
            font-family: sans-serif;
            text-transform: uppercase;
            outline: 0;
            background: #4CAF50;
            width: 100%;
            border: 0;
            padding: 15px;
            color: #FFFFFF;
            font-size: 14px;
            -webkit-transition: all 0.3 ease;
            transition: all 0.3 ease;
            cursor: pointer;
        }

        .form button:hover,
        .form button:active,
        .form button:focus {
            background: #43A047;
        }

        .form .message {
            margin: 15px 0 0;
            color: #b3b3b3;
            font-size: 12px;
        }

        .form .message a {
            color: #4CAF50;
            text-decoration: none;
        }

        .container {
            position: relative;
            z-index: 1;
            max-width: 300px;
            margin: 0 auto;
        }

        .container:before,
        .container:after {
            content: "";
            display: block;
            clear: both;
        }

        .container .info {
            margin: 50px auto;
            text-align: center;
        }

        .container .info h1 {
            margin: 0 0 15px;
            padding: 0;
            font-size: 36px;
            font-weight: 300;
            color: #1a1a1a;
        }

        .container .info span {
            color: #4d4d4d;
            font-size: 12px;
        }

        .container .info span a {
            color: #000000;
            text-decoration: none;
        }

        .container .info span .fa {
            color: #EF3B3A;
        }

        body {
            /* fallback for old browsers */
            background: #76b852;
            background: -webkit-linear-gradient(right, #76b852, #8DC26F);
            background: -moz-linear-gradient(right, #76b852, #8DC26F);
            background: -o-linear-gradient(right, #76b852, #8DC26F);
            background: linear-gradient(to left, #76b852, #8DC26F);

            font-family: sans-serif;

            -webkit-font-smoothing: antialiased;
            -moz-osx-font-smoothing: grayscale;
        }

        h1 {
            margin-top: 0;
        }

        .container-spinner {
            background: #4caf50;
            display: none;
            height: 47px;
        }

        /* Spinner */
        .loader {
            font-size: 3px;
            margin: auto;
            text-indent: -9999em;
            width: 11em;
            height: 11em;
            border-radius: 50%;
            background: #ffffff;
            background: -moz-linear-gradient(left, #ffffff 10%, rgba(255, 255, 255, 0) 42%);
            background: -webkit-linear-gradient(left, #ffffff 10%, rgba(255, 255, 255, 0) 42%);
            background: -o-linear-gradient(left, #ffffff 10%, rgba(255, 255, 255, 0) 42%);
            background: -ms-linear-gradient(left, #ffffff 10%, rgba(255, 255, 255, 0) 42%);
            background: linear-gradient(to right, #ffffff 10%, rgba(255, 255, 255, 0) 42%);
            position: relative;
            -webkit-animation: load3 1.4s infinite linear;
            animation: load3 1.4s infinite linear;
            -webkit-transform: translateZ(0);
            -ms-transform: translateZ(0);
            transform: translateZ(0);
        }

        .loader:before {
            width: 50%;
            height: 50%;
            background: #ffffff;
            border-radius: 100% 0 0 0;
            position: absolute;
            top: 0;
            left: 0;
            content: '';
        }

        .loader:after {
            background: #4caf50;
            width: 75%;
            height: 75%;
            border-radius: 50%;
            content: '';
            margin: auto;
            position: absolute;
            top: 0;
            left: 0;
            bottom: 0;
            right: 0;
        }

        @-webkit-keyframes load3 {
            0% {
                -webkit-transform: rotate(0deg);
                transform: rotate(0deg);
            }

            100% {
                -webkit-transform: rotate(360deg);
                transform: rotate(360deg);
            }
        }

        @keyframes load3 {
            0% {
                -webkit-transform: rotate(0deg);
                transform: rotate(0deg);
            }

            100% {
                -webkit-transform: rotate(360deg);
                transform: rotate(360deg);
            }
        }

        .d-none {
            display: none;
        }
        .d-block {
            display: block;
        }

        .alert {
            padding: 15px;
            margin-bottom: 20px;
            color: white;
            text-align: left;
        }

        .alert-success {
            background-color: #007bff;
        }

        .alert-error {
            background-color: #dc3545;
        }

        .closebtn {
            margin-left: 15px;
            color: white;
            font-weight: bold;
            float: right;
            font-size: 22px;
            line-height: 20px;
            cursor: pointer;
            transition: 0.3s;
        }

        .closebtn:hover {
            color: black;
        }
    </style>
</head>

<body>
    <div class="login-page">
        <div class="form">
            <div class="alert d-none" id="alert">
                <span class="closebtn" onclick="this.parentElement.classList.add('d-none');">&times;</span>
                <span></span>
            </div>
            <form class="login-form" id="wifi-form">
                <h1>WiFi</h1>
                <input type="text" id="ssid" name="ssid" placeholder="SSID" required />
                <input type="password" id="password" name="password" placeholder="Contraseña" />
                <button type="submit" id="submit-button">Connect to WiFi</button>
                <div class="container-spinner" id="spinner">
                    <div class="loader">Loading...</div>
                </div>
            </form>
        </div>
    </div>
    <script type="text/javascript">
        function onConnect(event) {
            event.preventDefault();
            document.getElementById("submit-button").style.display = "none";
            document.getElementById("spinner").style.display = "flex";
            let onAlert = document.getElementById("alert");
            if (onAlert != null) onAlert.style.display = "none";
            setTimeout(() => {
                document.getElementById("wifi-form").submit();
            }, 1000);
        }

        window.addEventListener("load", function () {
            function sendData() {
                const XHR = new XMLHttpRequest();

                // Bind the FormData object and the form element
                const FD = new FormData(form);

                // Define what happens on successful data submission
                XHR.addEventListener("load", function (event) {
                    console.log(event);
                    document.getElementById("spinner").style.display = "none";
                    document.getElementById("submit-button").style.display = "block";
                    let alertElem = document.getElementById("alert");
                    alertElem.classList.remove("d-none");
                    alertElem.classList.add("alert-success");
                    alertElem.getElementsByTagName('span')[1].innerHTML = "Conexion exitosa!";
                });

                // Define what happens in case of error
                XHR.addEventListener("error", function (event) {
                    alert('Oops! Something went wrong.');
                    document.getElementById("spinner").style.display = "none";
                    document.getElementById("submit-button").style.display = "block";
                    let alertElem = document.getElementById("alert");
                    alertElem.classList.remove("d-none");
                    alertElem.classList.add("alert-error");
                    alertElem.getElementsByTagName('span')[1].innerHTML = "Ocurrio un error inesperado...";
                });

                // Set up our request
                XHR.open("POST", "connect");

                // The data sent is what the user provided in the form
                XHR.send(FD);
            }

            // Access the form element...
            var form = document.getElementById("wifi-form");

            // ...and take over its submit event.
            form.addEventListener("submit", function (event) {
                event.preventDefault();
                document.getElementById("submit-button").style.display = "none";
                document.getElementById("spinner").style.display = "flex";
                document.getElementById("alert").classList.add('d-none');
                setTimeout(() => {
                    sendData();
                }, 1000);
            });
        });
    </script>
</body>

</html>)";
#endif

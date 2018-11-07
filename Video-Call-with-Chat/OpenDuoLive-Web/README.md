# OpenDuoLive-Web

This Agora Tutorial is an open-source demo that will help you get live video broadcasting and chat messaging integrated directly into your JS applications using the Agora Video SDK + Signaling SDK.

With this sample app, you can:

- Join broadcast as an broadcasting member
- Join broadcast as an audience member
- Chat with broadcasters and viewers

## Running the App

The Video SDK and Signaling SDK are already included in this demo app inside 'src/utils'.

Create a developer account at [Agora.io](https://dashboard.agora.io/signin/), and obtain an App ID. Update "agora.config.js" inside the 'static' folder with your App ID.

```
const AGORA_APP_ID = 'ADD-APP-ID-HERE'
```

Run the following commands

```
npm install
npm audit fix
npm run build
cd dist

// SSL & Deploy to HTTPS
openssl req -newkey rsa:2048 -nodes -keyout key.pem -x509 -days 365 -out certificate.pem
sudo http-server ./ -p 443 --ssl -C certificate.pem -K key.pem
```

## Developer Environment Requirements
* NPM
* http-server (use SSL to enable deployment to https)

## Connect Us

- You can find full API document at [Document Center](https://docs.agora.io/en/)
- You can file bugs about this demo at [issue](https://github.com/AgoraIO/Advanced-Video/issues)

## License

The MIT License (MIT).

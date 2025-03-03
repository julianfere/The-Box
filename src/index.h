
const char mainPage[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SpotifyBuddy</title>
    <style>
        body {
            font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif;
            background-color: #121212;
            color: #fff;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            flex-direction: column;
            text-align: center;
        }

        h1 {
            font-size: 3rem;
            font-weight: 700;
            color: #fff;
            margin-bottom: 40px;
        }

        a {
            display: inline-block;
            background-color: #1DB954;
            color: #fff;
            font-size: 1.2rem;
            font-weight: 600;
            text-decoration: none;
            padding: 15px 40px;
            border-radius: 50px;
            transition: background-color 0.3s, transform 0.3s;
        }

        a:hover {
            background-color: #1ed760;
            transform: scale(1.05);
        }

        a:active {
            background-color: #1DB954;
        }
    </style>
</head>
<body>
    <h1>SpotifyBuddy</h1>
    <a href="https://accounts.spotify.com/authorize?response_type=code&client_id=%client_id%&redirect_uri=%redirect_uri%&scope=user-modify-playback-state user-read-currently-playing user-read-playback-state user-library-modify user-library-read">Connect with Spotify</a>
</body>
</html>
  )=====";

const char errorPage[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SpotifyBuddy</title>
    <style>
        body {
            font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif;
            background-color: #121212;
            color: #fff;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            flex-direction: column;
            text-align: center;
        }

        h1 {
            font-size: 3rem;
            font-weight: 700;
            color: #fff;
            margin-bottom: 40px;
        }

        a {
            display: inline-block;
            background-color: #1DB954;
            color: #fff;
            font-size: 1.2rem;
            font-weight: 600;
            text-decoration: none;
            padding: 15px 40px;
            border-radius: 50px;
            transition: background-color 0.3s, transform 0.3s;
        }

        a:hover {
            background-color: #1ed760;
            transform: scale(1.05);
        }

        a:active {
            background-color: #1DB954;
        }
    </style>
</head>
<body>
    <h1>SpotifyBuddy</h1>
    <h2>Something went wrong</h2>
    <a href="https://accounts.spotify.com/authorize?response_type=code&client_id=%client_id%&redirect_uri=%redirect_uri%&scope=user-modify-playback-state user-read-currently-playing user-read-playback-state user-library-modify user-library-read">Connect with Spotify</a>
</body>
</html>
  )=====";
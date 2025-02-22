const char *CAPTIVE_PAGE = R"=====(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>The Box</title>
</head>
<style>
  * {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
  }

  body {
    font-family: 'Roboto', sans-serif;
    background: #f0f2f5;
  }

  main {
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    height: 100vh;
    background: linear-gradient(145deg, #2c3e50, #34495e);
    color: white;
    text-align: center;
    padding: 20px;
  }

  h1 {
    font-size: 2rem;
    font-weight: 500;
    margin-bottom: 2rem;
    max-width: 600px;
    line-height: 1.5;
  }

  .redirect-btn {
    color: white;
    padding: 0.75rem 2rem;
    background: #3498db;
    font-weight: 500;
    border-radius: 25px;
    border: none;
    cursor: pointer;
    transition: background 0.3s ease;
    text-transform: uppercase;
    font-size: 0.9rem;
    letter-spacing: 1px;
  }

  .redirect-btn:hover {
    background: #2980b9;
  }

</style>
<body>
  <main>
    <h1>Para la utilización del equipo se requiere un WiFi con conexión a internet.</h1>
    <a href="/home"><button class="redirect-btn">Configurar Red</button></a>
  </main>
</body>
</html>
)=====";

const char *MAIN_PAGE_1 = R"=====(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>The Box - Configuración WiFi</title>
  <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500;700&display=swap" rel="stylesheet">
</head>
<style>
  * {
    padding: 0;
    margin: 0;
    box-sizing: border-box;
  }

  body {
    font-family: 'Roboto', sans-serif;
    background: #f0f2f5;
  }

  main {
    background: linear-gradient(145deg, #2c3e50, #34495e);
    height: 100vh;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    color: white;
    text-align: center;
    padding: 20px;
  }

  .content-image img {
    width: 100px;
    height: auto;
    margin-bottom: 1.5rem;
  }

  h1 {
    font-size: 1.75rem;
    font-weight: 400;
    margin-bottom: 2rem;
    max-width: 600px;
    line-height: 1.5;
  }

  form {
    display: flex;
    flex-direction: column;
    gap: 1.5rem;
    width: 100%;
    max-width: 300px;
  }

  select, input {
    padding: 0.75rem;
    width: 100%;
    border-radius: 25px;
    border: none;
    font-size: 1rem;
    text-align: center;
    background: #ecf0f1;
    color: #2c3e50;
    transition: background 0.3s ease;
  }

  select:focus, input:focus {
    outline: none;
    background: #ffffff;
  }

  .form-submit {
    background: #3498db;
    color: white;
    font-weight: 500;
    cursor: pointer;
    transition: background 0.3s ease;
    text-transform: uppercase;
    letter-spacing: 1px;
  }

  .form-submit:hover {
    background: #2980b9;
  }

  label {
    font-size: 0.9rem;
    margin-bottom: 0.5rem;
    color: #ecf0f1;
  }

</style>
<body>
  <main class="flex-column">
    <h1>Para la utilización del equipo se requiere un WiFi con conexión a internet.</h1>

    <form method='post' action='connect' class="form-wifi">
      <section class="flex-column">
        <label for="network">Red</label>
        <select id="network" name="ssid">
          <option value="">--- Seleccione una red ---</option>
)=====";

const char *MAIN_PAGE_2 = R"=====(
        </select>
      </section>
      <section class="flex-column">
        <label for="password">Contraseña</label>
        <input id="password" name='password' type="password" length=64>
      </section>
      <section class="flex-column">
        <input class="form-submit" type='submit' value="Conectar">
      </section>
    </form>
  </main>
</body>
</html>
)=====";

const char *CONNECTING_PAGE = R"=====(
<!DOCTYPE html>
<html lang="es">

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>The Box - Estado de Conexión</title>
  <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@300;400;500;700&display=swap" rel="stylesheet">
</head>
<style>
  * {
    padding: 0;
    margin: 0;
    box-sizing: border-box;
  }

  body {
    font-family: 'Roboto', sans-serif;
    background: #f0f2f5;
  }

  main {
    background: linear-gradient(145deg, #2c3e50, #34495e);
    height: 100vh;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    color: white;
    text-align: center;
    padding: 20px;
  }

  h2 {
    font-size: 2rem;
    font-weight: 500;
    margin-bottom: 1rem;
  }

  .flex-column {
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
    gap: 1rem;
  }
</style>

<body>
  <main class="flex-column">
    <h2>Conectando...</h2>
  </main>
</body>

</html>
)=====";
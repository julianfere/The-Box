const char *CAPTIVE_PAGE = R"=====(
<!DOCTYPE html>
<html lang="es">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <title>CultBot</title>
</head>
<style>

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

main {
  display: flex;
  flex-direction: column;
  justify-content: space-around;
  align-items: center;
  height: 100vh;
  background: linear-gradient(10deg, rgb(151, 189, 3) 50%, rgb(39, 159, 128) 100%);
}

h1 {
  color: white;
  text-align: center;
  font-weight: 600;
  font-family: 'Montserrat', sans-serif;
}

.redirect-btn {
  color: black;
  padding: 0.5rem 1rem 0.5rem 1rem;
  text-align: center;
  background: rgb(39, 159, 128);
  font-weight: 900;
  border-radius: 5px;
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
  <title>CultBot</title>
</head>
<style>
* {
  padding: 0;
  margin: 0;
  box-sizing: border-box;
}

main {
  background: linear-gradient(10deg, rgb(151, 189, 3) 50%, rgb(39, 159, 128) 100%);
  height: 100vh;
}

.flex-column {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
}

form {
  display: flex;
  flex-direction: column;
  gap: 1.5rem;
}

select, input {
  padding: 0.5rem;
  min-width: 20rem;
  border-radius: 20px;
  text-align: center;
}
</style>
<body>
  <main class="flex-column">
    <div class="content-image"><img src="culbot" alt="CultBot Image"></div>
    <h1>Para la utilización del equipo se requiere un WiFi con conexión a internet.</h1>

    <form method='post' action='connect' class="form-wifi">
      <section class="flex-column">
        <label for="network">Red</label>
        <select id="network" name="ssid">
          <option value=""> --- Seleccione una red --- </option>
)=====";

const char *MAIN_PAGE_2 = R"=====(
        </select>
      </section>
      <section class="flex-column">
        <label for="password">Password</label>
        <input id="password" name='password' type="password" length=64>
      </section>
      <section class="flex-column">
        <input class="form-submit" type='submit'>
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
  <title>CultBot</title>
</head>
<style>
* {
  padding: 0;
  margin: 0;
  box-sizing: border-box;
}

main {
  background: linear-gradient(10deg, rgb(151, 189, 3) 50%, rgb(39, 159, 128) 100%);
  height: 100vh;
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
    <div class="content-image"><img src="culbot" alt="CultBot Image"></div>
    <h2>Conectando...</h2>
    <h3>Puede ver el estado de la conexion en el primer led</h3>
    <h3>Si es rojo NO se pudo conectar</h3>
    <h3>Si es verde SI se pudo conectar</h3>
    <h3>Si es parpadea en verde se esta conectando</h3>
  </main>
</body>

</html>
)=====";
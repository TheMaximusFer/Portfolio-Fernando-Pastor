import { jwtDecode } from "jwt-decode";

// URL con la de la DB
const API_BASE_URL = "/api/";

const montarFiltrosMundo = (filters) => {
  const params = new URLSearchParams();

  if (filters) {
    // Asumiendo que el backend espera 'version' y 'platform'
    if (filters.version) {
      params.append("version", filters.version);
    }

    if (filters.platform == "Java") {
      params.append("java", "true");
    } else {
      params.append("java", "false");
    }

    if (filters.types) {
      Object.keys(filters.types).forEach((typeKey) => {
        if (filters.types[typeKey] === true) {
          params.append(typeKey, "true");
        }
      });
    }

    if (filters.search != "") {
      params.append("search", filters.search);
    }
  }
  return params.toString();
};

/*
 *   FUNCION DE INICIO DE SESION
 */
export const handleLogin = async (UsuarioDTO) => {
  try {
    // 1. Await: Pausa aquí hasta que 'fetch' complete
    const LOGIN_URL = "auth/login";
    const respuesta = await fetch(`${API_BASE_URL}${LOGIN_URL}`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(UsuarioDTO),
    });

    // 2. Verifica si la respuesta HTTP fue exitosa (ej: 200 OK)
    if (!respuesta.ok) {
      // Si no fue exitosa (ej: 401 Unauthorized), lanza un error
      // para ser atrapado por el bloque 'catch'.
      console.error("Credenciales incorrectas (Respuesta no OK)");
      // setMessage("Usuario o contraseña incorrectos");
      throw new Error("Usuario o contraseña incorrectos");
    }

    // 3. Await: Pausa aquí hasta que el cuerpo de la respuesta se lea como texto
    const token = await respuesta.text();

    // 4. Procesa el token si existe
    if (token) {
      console.log("Login exitoso!, token JWT: ", token);
      localStorage.setItem("jwtToken", token);
      // setMessage("Sesión iniciada correctamente");

      // (Buena práctica: devuelve el token o true para que quien llame sepa que fue exitoso)
      return token;
    } else {
      // Si la respuesta fue OK pero el token está vacío
      throw new Error("Respuesta OK pero no se recibió token.");
    }
  } catch (error) {
    // 5. Atrapa errores de red (ej. servidor caído) O los errores que lanzamos (ej. !respuesta.ok)
    console.error("Error en el login:", error.message);
    // (Devuelve null o false para que quien llame sepa que falló)
    return null;
  }
};

/*
 * FUNCIONES DE FETCH
 */

const fetchFromDB = async (url) => {
  console.log("Llamando a Spring Boot con URL:", url);
  const header = {};
  const token = localStorage.getItem("jwtToken");

  if (token) {
    header["Authorization"] = `Bearer ${token}`;
    console.log("Si hay token");
  }

  try {
    //Envía una solicitud GET
    const response = await fetch(url, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        ...header,
      },
    });

    // 1. Manejo de errores HTTP (códigos 4xx o 5xx)
    if (!response.ok) {
      // Lanza un error si la respuesta no es exitosa (ej: 404, 500)
      throw new Error(`HTTP error! status: ${response.status}`);
    }

    // 2. Parsear el cuerpo de la respuesta como JSON
    const data = await response.json();

    return await data;
  } catch (error) {
    throw error;
  }
};

export const fetchMaps = async (filters) => {
  const filtros = montarFiltrosMundo(filters);
  var db_folder = "worlds/public";

  const url = `${API_BASE_URL}${db_folder}?${filtros}`;

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchMyMaps = async (filters) => {
  const queryString = montarFiltrosMundo(filters);
  var db_folder = "worlds/my-worlds";
  var url = "";

  if (filters) {
    url = `${API_BASE_URL}${db_folder}?${queryString}`;
  } else {
    url = `${API_BASE_URL}${db_folder}`;
  }

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchUserMaps = async (user, map) => {
  if (user == getUsernameFromToken()) {
    var db_folder = `worlds/my-worlds`;
  } else {
    var db_folder = `worlds`;
  }

  var queryString = `?username=${user}`;
  if (map != null) {
    queryString = `${queryString}&world=${map}`;
  }

  const url = `${API_BASE_URL}${db_folder}${queryString}`;

  try {
    return await fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchMyFavourites = async (filters) => {
  const queryString = montarFiltrosMundo(filters);
  var db_folder = "favourites";

  const url = `${API_BASE_URL}${db_folder}?${queryString}`;

  try {
    return await fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchAllMyFavourites = async () => {
  var db_folder = "favourites/no-filters";

  const url = `${API_BASE_URL}${db_folder}`;

  try {
    console.log("HOlA BUENOS DIAS AQUI TIENE");
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchUsers = async () => {
  const DB_FOLDER = "users";
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchUserByUsername = async (username) => {
  const DB_FOLDER = `users?username=${username}`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchMarkers = async (owner, world) => {
  var author = owner;
  if (owner == getUsernameFromToken()) {
    author = "my-worlds";
  }
  const DB_FOLDER = `worlds/${author}/${world}/markers`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener mapas de la API:", error);
    throw new Error("No se pudieron obtener los mapas desde el servidor.");
  }
};

export const fetchReports = async () => {
  const DB_FOLDER = `reports`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error al obtener denuncias de la API:", error);
    throw new Error("No se pudieron obtener las denuncias desde el servidor.");
  }
};

export const downloadFile = async (url, filename) => {
  try {
    const header = {};
    const token = localStorage.getItem("jwtToken");

    if (token) {
      header["Authorization"] = `Bearer ${token}`;
    }

    const response = await fetch(url, {
      method: "GET",
      headers: {
        ...header,
      },
    });

    if (!response.ok) {
      throw new Error(`Error al descargar: ${response.statusText}`);
    }

    // 1. Convertimos la respuesta en un Blob (datos binarios)
    const blob = await response.blob();

    // 2. Creamos una URL temporal que apunta a ese Blob en la memoria del navegador
    const urlBlob = window.URL.createObjectURL(blob);

    // 3. Creamos un elemento <a> invisible para forzar la descarga
    const a = document.createElement("a");
    a.href = urlBlob;
    a.download = filename; // El nombre con el que se guardará el archivo
    document.body.appendChild(a); // Necesario en Firefox
    a.click(); // Simulamos el clic
    a.remove(); // Limpiamos el DOM

    // 4. Liberamos la memoria del navegador
    window.URL.revokeObjectURL(urlBlob);
  } catch (error) {
    console.error("Error en la descarga:", error);
  }
};

export const fetchWorldImages = async (worldOwner, worldName) => {
  var author = worldOwner;
  if (worldOwner == getUsernameFromToken()) {
    author = "my-worlds";
  }
  var db_folder = `worlds/${author}/${worldName}/photos`;

  const url = `${API_BASE_URL}${db_folder}`;

  try {
    return fetchFromDB(url);
  } catch (error) {
    console.error("Error las imagenes:", error);
    throw new Error("No se pudieron obtener las imagenes desde el servidor.");
  }
};

export const fetchImageForDisplay = async (url) => {
  try {
    const header = {};
    const token = localStorage.getItem("jwtToken");

    if (token) {
      header["Authorization"] = `Bearer ${token}`;
    }

    // 2. Hacemos la petición
    const response = await fetch(url, {
      method: "GET",
      headers: {
        ...header,
      },
    });

    if (!response.ok) {
      throw new Error(`Error ${response.status}: No se pudo cargar la imagen`);
    }

    const blob = await response.blob();

    const objectUrl = window.URL.createObjectURL(blob);

    return objectUrl;
  } catch (error) {
    console.error(`Fallo al cargar imagen de ${url}:`, error);
    // Retornamos null para que el componente pueda mostrar una imagen por defecto
    return null;
  }
};

/*
 * FUNCIONES DE POST
 */

const postToDB = async (url, data) => {
  try {
    const header = {};
    const token = localStorage.getItem("jwtToken");

    if (token) {
      header["Authorization"] = `Bearer ${token}`;
    }

    const response = await fetch(url, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        ...header,
      },
      body: JSON.stringify(data),
    });

    if (!response.ok) {
      // Manejar errores como 400 Bad Request o 500 Internal Server Error
      const errorBody = await response.json();
      throw new Error(
        errorBody.message || `HTTP error! status: ${response.status}`
      );
    }

    // Asumimos que Spring Boot devuelve el objeto creado o un mensaje JSON
    return await response.json();
  } catch (error) {
    throw error;
  }
};

export const postWorld = async (data) => {
  const DB_FOLDER = "worlds/my-worlds";
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await postToDB(url, data);
  } catch (error) {
    console.error(`Error al insertar en maps:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postUser = async (data) => {
  const DB_FOLDER = "auth/register";
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await postToDB(url, data);
  } catch (error) {
    console.error(`Error al insertar en users:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postReport = async (data) => {
  const DB_FOLDER = `reports`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await postToDB(url, data);
  } catch (error) {
    console.error(`Error al insertar en users:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postMarker = async (worldName, data) => {
  const DB_FOLDER = `worlds/my-worlds/${worldName}/markers`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await postToDB(url, data);
  } catch (error) {
    console.error(`Error al insertar en users:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postFavourite = async (owner, world) => {
  const DB_FOLDER = `favourites`;
  const url = `${API_BASE_URL}${DB_FOLDER}/${owner}/${world}/favourite`;

  try {
    return await postToDB(url, "");
  } catch (error) {
    console.error(`Error al insertar en favoritos:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postFile = async (url, file) => {
  const formData = new FormData();
  formData.append("file", file);

  const header = {};
  const token = localStorage.getItem("jwtToken");

  if (token) {
    header["Authorization"] = `Bearer ${token}`;
  }

  try {
    const response = await fetch(url, {
      method: "POST",
      headers: {
        ...header,
      },
      body: formData,
    });

    if (response.ok) {
      console.log("Archivo subido correctamente");
    }

    return await response.json();
  } catch (error) {
    console.error("Error de red:", error);
  }
};

export const postWorldImage = async (worldName, data) => {
  const DB_FOLDER = `worlds/my-worlds/${worldName}/photos`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  console.log("Subiendo imagen al mundo");

  try {
    return await postFile(url, data);
  } catch (error) {
    console.error(`Error al insertar imagenes en el mundo:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postWorldFile = async (worldName, data) => {
  const DB_FOLDER = `worlds/my-worlds/${worldName}/file`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await postFile(url, data);
  } catch (error) {
    console.error(`Error al insertar imagenes en el mundo:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

export const postMarkerImage = async (markerName, worldName, data) => {
  const DB_FOLDER = `worlds/my-worlds/${worldName}/markers/${markerName}/image`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await postFile(url, data);
  } catch (error) {
    console.error(`Error al insertar la imagenen en el marcador:`, error);
    throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
  }
};

/*
 * FUNCIONES DE DELETE
 */

const deleteFromDB = async (url) => {
  try {
    const header = {};
    const token = localStorage.getItem("jwtToken");

    if (token) {
      header["Authorization"] = `Bearer ${token}`;
    }

    const response = await fetch(url, {
      method: "DELETE",
      headers: {
        "Content-Type": "application/json",
        ...header,
      },
    });

    if (!response.ok) {
      // Manejar errores como 400 Bad Request o 500 Internal Server Error
      const errorBody = await response.json();
      throw new Error(
        errorBody.message || `HTTP error! status: ${response.status}`
      );
    }

    // Asumimos que Spring Boot devuelve el objeto creado o un mensaje JSON
    const text = await response.text();
    return text ? JSON.parse(text) : {};
  } catch (error) {
    throw error;
  }
};

export const deleteUser = async (user) => {
  const USER = user;
  const DB_FOLDER = "users/";
  const url = `${API_BASE_URL}${DB_FOLDER}${USER}`;

  try {
    deleteFromDB(url);
  } catch (error) {
    console.error(`Error al eliminar en users:`, error);
    throw new Error(`Fallo al conectar con el servidor para eliminar datos.`);
  }
};

export const deleteFavourite = async (owner, world) => {
  const DB_FOLDER = `favourites`;
  const url = `${API_BASE_URL}${DB_FOLDER}/${owner}/${world}/favourite`;

  try {
    return await deleteFromDB(url);
  } catch (error) {
    console.error(`Error al borrar en favoritos:`, error);
    throw new Error(`Fallo al conectar con el servidor para borrar datos.`);
  }
};

export const deleteReport = async (reporter, reported) => {
  const DB_FOLDER = `reports`;
  const url = `${API_BASE_URL}${DB_FOLDER}/${reporter}/${reported}`;

  try {
    return await deleteFromDB(url);
  } catch (error) {
    console.error(`Error al borrar en favoritos:`, error);
    throw new Error(`Fallo al conectar con el servidor para borrar datos.`);
  }
};

export const deleteWorld = async (worldName) => {
  const DB_FOLDER = `worlds/my-worlds`;
  const url = `${API_BASE_URL}${DB_FOLDER}/${worldName}`;

  try {
    return await deleteFromDB(url);
  } catch (error) {
    console.error(`Error al borrar en mis mundos:`, error);
    throw new Error(`Fallo al conectar con el servidor para borrar datos.`);
  }
};

// FUNCIONES DE TOKEN

const decodifyToken = () => {
  const token = localStorage.getItem("jwtToken");
  if (!token) return null;

  try {
    const payload = jwtDecode(token);

    // El payload ya es un objeto JSON decodificado.
    return payload;
  } catch (e) {
    // Esto captura errores si el token está mal formado o no es un JWT válido
    console.error("Error decodificando JWT:", e);
    return null;
  }
};

export const getUsernameFromToken = () => {
  try {
    const payload = decodifyToken();

    //Verificamos si payload existe antes de pedir .sub
    if (payload && payload.sub) {
      return payload.sub;
    }
    return null; // Si no hay payload o sub, devolvemos null sin explotar
  } catch (e) {
    // Si el token es inválido o la decodificación falla, devolvemos null
    console.error("Error decodificando JWT:", e);
    return null;
  }
};

/* src/services/apiService.js */


const updateToDB = async (url, data) => {
  try {
    const header = {};
    const body = {}
    const token = localStorage.getItem("jwtToken");

    if (token) {
      header["Authorization"] = `Bearer ${token}`;
    }

    if (data) {
      body["body"] = JSON.stringify(data);
    }

    const response = await fetch(url, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
        ...header,
      },
      ...body
    });

    if (!response.ok) {
      // Manejar errores como 400 Bad Request o 500 Internal Server Error
      const errorBody = await response.json();
      throw new Error(
        errorBody.message || `HTTP error! status: ${response.status}`
      );
    }

    const text = await response.text(); // Leemos como texto primero
    if (text) {
        data = JSON.parse(text); // Solo parseamos si el texto no esta vacio
        return data
    }
    return text
  } catch (error) {
    throw error;
  }
};

export const updateUserProfile = async ( data, file) => {

  const urlUserData = `${API_BASE_URL}users`;
  const urlUserPicture= `${API_BASE_URL}users/me/profile-picture`;
  var dataResponse = ""
  var pictureResponse = ""

  try {
    
    dataResponse = await updateToDB(urlUserData, data)
  
    if (file) {
      
      pictureResponse = await postFile(urlUserPicture, file)

    }

    return { dataResponse, pictureResponse };
  } catch (error) {
    console.error("Error en updateUserProfile:", error);
    throw error;
  }
};


// Descarga una imagen autenticada y la convierte en un objeto File
export const fetchImageAsFile = async (url, filename) => {
  try {
    const header = {};
    const token = localStorage.getItem("jwtToken");

    if (token) {
      header["Authorization"] = `Bearer ${token}`;
    }

    const response = await fetch(url, {
      method: "GET",
      headers: { ...header },
    });

    if (!response.ok) throw new Error("Error descargando imagen");

    const blob = await response.blob();
    
    // Creamos un archivo JS a partir del Blob descargado
    // Le asignamos el tipo MIME correcto (ej: image/png)
    return new File([blob], filename, { type: blob.type });

  } catch (error) {
    console.error("Error convirtiendo imagen a archivo:", error);
    return null;
  }
};

export const postBan = async (bannedUser, data) => {
  const DB_FOLDER = `admin/ban/${bannedUser}`;
  const url = `${API_BASE_URL}${DB_FOLDER}`;

  try {
    return await updateToDB(url, data); 
  } catch (error) {
    console.error("Error al sancionar:", error);
    throw error;
  }
};

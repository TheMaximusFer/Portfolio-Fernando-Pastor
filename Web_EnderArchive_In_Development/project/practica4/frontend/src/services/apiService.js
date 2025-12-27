import { jwtDecode } from "jwt-decode";

// URL con la de la DB
const API_BASE_URL = '/api/'; 

const montarFiltrosMundo = (filters) => {
    const params = new URLSearchParams();

    if (filters) {
        // Asumiendo que el backend espera 'version' y 'platform'
        if (filters.version) {
            params.append('version', filters.version); 
        }

        if (filters.platform == "Java") {
            params.append('java', 'true'); 
        } else {
            params.append('java', 'false');
        }

        if (filters.types) {
            Object.keys(filters.types).forEach(typeKey => {
            if (filters.types[typeKey] === true) {
                params.append(typeKey, 'true'); 
            }
        });
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
    const LOGIN_URL = 'auth/login'
    const respuesta = await fetch(`${API_BASE_URL}${LOGIN_URL}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(UsuarioDTO)
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
      localStorage.setItem('jwtToken', token);
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
    const token = localStorage.getItem('jwtToken');

    if (token) {
        header['Authorization'] = `Bearer ${token}`; 
        console.log("Si hay token")
    }  

    try {
        //Envía una solicitud GET
        const response = await fetch(url, {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json',
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
        
        return data; 

    } catch (error) {
        throw error;
    }

};

export const fetchMaps = async (filters) => {
    const filtros = montarFiltrosMundo(filters);
    var db_folder = 'worlds/public'
   
    
    const url = `${API_BASE_URL}${db_folder}?${filtros}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor."); 
    }
};

export const fetchMyMaps = async (filters) => {
    const queryString = montarFiltrosMundo(filters);
    var db_folder = 'worlds/my-worlds'
    var url = '';

    if(filters){
        url = `${API_BASE_URL}${db_folder}?${queryString}`;
    }else{
        url = `${API_BASE_URL}${db_folder}`;
    }
    

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor."); 
    }
};

export const fetchUserMaps = async (user, map) => {
    if (user == getUsernameFromToken()) {
        var db_folder = `worlds/my-worlds`
    } else {
        var db_folder = `worlds`
    }

    var queryString = `?username=${user}`;
    if (map != null) {
        queryString = `${queryString}&world=${map}`
    }
    
    const url = `${API_BASE_URL}${db_folder}${queryString}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor."); 
    }
};

export const fetchMyFavourites = async (filters) => {
    const queryString = montarFiltrosMundo(filters);
    var db_folder = 'favourites'
   

    const url = `${API_BASE_URL}${db_folder}?${queryString}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor."); 
    }
};

export const fetchAllMyFavourites = async () => {
    var db_folder = 'favourites/no-filters'
   

    const url = `${API_BASE_URL}${db_folder}`;

    try {
        console.log("HOlA BUENOS DIAS AQUI TIENE")
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor."); 
    }
};

export const fetchUsers = async () => {
    const DB_FOLDER = 'users';
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor.");
    }
};

export const fetchUserByUsername = async ( username ) => {
    const DB_FOLDER = `users?username=${username}`;
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor.");
    }
}

export const fetchMarkers = async ( owner, world ) => {
    var author = owner
    if (owner == getUsernameFromToken()) {
        author = 'my-worlds'
    }
    const DB_FOLDER = `worlds/${author}/${world}/markers`;
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener mapas de la API:", error);
        throw new Error("No se pudieron obtener los mapas desde el servidor.");
    }
}

export const fetchReports = async () => {
    const DB_FOLDER = `reports`;
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return fetchFromDB(url)
    } catch (error) {
        console.error("Error al obtener denuncias de la API:", error);
        throw new Error("No se pudieron obtener las denuncias desde el servidor.");
    }
}

/*
 * FUNCIONES DE POST
 */

const postToDB = async (url, data) => {
    
    try {

        const header = {};
        const token = localStorage.getItem('jwtToken');

        if (token) {
            header['Authorization'] = `Bearer ${token}`; 
        } 

        const response = await fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                ...header,
            },
            body: JSON.stringify(data), 
        });

        if (!response.ok) {
            // Manejar errores como 400 Bad Request o 500 Internal Server Error
            const errorBody = await response.json();
            throw new Error(errorBody.message || `HTTP error! status: ${response.status}`);
        }
        
        // Asumimos que Spring Boot devuelve el objeto creado o un mensaje JSON
        return await response.json();

    } catch (error) {
        throw error;
    }
};

export const postWorld = async (data) => {

    const DB_FOLDER = 'worlds/my-worlds';
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return await postToDB(url, data)
    } catch (error) {
        console.error(`Error al insertar en maps:`, error);
        throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
    }

};

export const postUser = async (data) => {

    const DB_FOLDER = 'auth/register';
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return await postToDB(url, data)
    } catch (error) {
        console.error(`Error al insertar en users:`, error);
        throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
    }

};

export const postReport = async (data) => {

    const DB_FOLDER = `reports`;
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return await postToDB(url, data)
    } catch (error) {
        console.error(`Error al insertar en users:`, error);
        throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
    }

};

export const postMarker = async (worldName, data) => {

    const DB_FOLDER = `worlds/my-worlds/${worldName}/markers`;
    const url = `${API_BASE_URL}${DB_FOLDER}`;

    try {
        return await postToDB(url, data)
    } catch (error) {
        console.error(`Error al insertar en users:`, error);
        throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
    }

};

export const postFavourite = async ( owner, world ) => {

    const DB_FOLDER = `favourites`;
    const url = `${API_BASE_URL}${DB_FOLDER}/${owner}/${world}/favourite`;

    try {
        return await postToDB(url, "")
    } catch (error) {
        console.error(`Error al insertar en favoritos:`, error);
        throw new Error(`Fallo al conectar con el servidor para insertar datos.`);
    }

};

export const postFile = async (url, file) => {

    const formData = new FormData();
    formData.append('file', file); 

    try {
      const response = await fetch(url, {
        method: 'POST',
        body: formData,
      });

      if (response.ok) {
        alert('Archivo subido correctamente');

      } else {
        alert('Error al subir');
      }
    } catch (error) {
      console.error('Error de red:', error);
    }
  };

/*
 * FUNCIONES DE DELETE
 */

const deleteFromDB = async (url) => {
    
    try {

        const header = {};
        const token = localStorage.getItem('jwtToken');

        if (token) {
            header['Authorization'] = `Bearer ${token}`; 
        } 

        const response = await fetch(url, {
            method: 'DELETE',
            headers: {
                'Content-Type': 'application/json',
                ...header,
            },
        });

        if (!response.ok) {
            // Manejar errores como 400 Bad Request o 500 Internal Server Error
            const errorBody = await response.json();
            throw new Error(errorBody.message || `HTTP error! status: ${response.status}`);
        }
        
        // Asumimos que Spring Boot devuelve el objeto creado o un mensaje JSON
        const text = await response.text();
        return text ? JSON.parse(text) : {};

    } catch (error) {
        throw error;
    }
};

export const deleteUser = async (user) => {

    const USER = user
    const DB_FOLDER = 'users/';
    const url = `${API_BASE_URL}${DB_FOLDER}${USER}`;

    try {
        deleteFromDB(url)
    } catch (error) {
        console.error(`Error al eliminar en users:`, error);
        throw new Error(`Fallo al conectar con el servidor para eliminar datos.`);
    }

};

export const deleteFavourite = async (owner, world) => {

    const DB_FOLDER = `favourites`;
    const url = `${API_BASE_URL}${DB_FOLDER}/${owner}/${world}/favourite`;

    try {
        return await deleteFromDB(url)
    } catch (error) {
        console.error(`Error al borrar en favoritos:`, error);
        throw new Error(`Fallo al conectar con el servidor para borrar datos.`);
    }

};

export const deleteReport = async ( reporter, reported) => {

    const DB_FOLDER = `reports`;
    const url = `${API_BASE_URL}${DB_FOLDER}/${reporter}/${reported}`;

    try {
        return await deleteFromDB(url)
    } catch (error) {
        console.error(`Error al borrar en favoritos:`, error);
        throw new Error(`Fallo al conectar con el servidor para borrar datos.`);
    }

}

// FUNCIONES DE TOKEN

const decodifyToken = () => {
    const token = localStorage.getItem('jwtToken');
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
    // const token = localStorage.getItem('jwtToken');
    // if (!token) return null;

    try {
        // // El payload es la segunda parte del token (índice 1)
        // const base64Url = token.split('.')[1];
        
        // // Conversión de Base64URL a Base64 estándar
        // const base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
        
        // // Decodificación y parseo a JSON
        // const jsonPayload = decodeURIComponent(atob(base64).split('').map(function(c) {
        //     return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2);
        // }).join(''));

        // const payload = JSON.parse(jsonPayload);
        
        // // Un JWT estándar usa 'sub' (subject) o 'username'
        // return payload.sub || payload.username; 

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


export const updateUserProfile = async (username, data, file) => {
    const token = localStorage.getItem('jwtToken');
    const headersJSON = {
        'Content-Type': 'application/json',
        'Authorization': `Bearer ${token}`
    };
    const headersMultipart = {
        'Authorization': `Bearer ${token}`
        // NO ponemos Content-Type aquí, el navegador lo pone solo con el Boundary correcto
    };

    try {
        // 1. ACTUALIZAR DATOS DE TEXTO (Si hay descripción)
        // Tu backend espera JSON en el PUT /api/users/{username}
        if (data && data.description !== undefined) {
            const urlText = `${API_BASE_URL}users/${username}`;
            
            const responseText = await fetch(urlText, {
                method: 'PUT',
                headers: headersJSON,
                body: JSON.stringify({ 
                    description: data.description 
                    // Añade aquí otros campos si tu backend lo permite (email, etc.)
                })
            });

            if (!responseText.ok) {
                throw new Error("Error al actualizar la descripción.");
            }
        }

        // 2. ACTUALIZAR FOTO (Si hay archivo)
        // Tu backend espera Multipart en el POST /api/users/me/profile-picture
        if (file) {
            const urlImg = `${API_BASE_URL}users/me/profile-picture`;
            const formData = new FormData();
            formData.append("file", file);

            const responseImg = await fetch(urlImg, {
                method: 'POST',
                headers: headersMultipart,
                body: formData
            });

            if (!responseImg.ok) {
                throw new Error("Error al subir la imagen.");
            }
        }

        return { success: true };

    } catch (error) {
        console.error("Error en updateUserProfile:", error);
        throw error;
    }
};
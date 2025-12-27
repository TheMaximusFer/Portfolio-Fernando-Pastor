import React, { useState, useEffect, useMemo, useRef } from "react";
import Header from "../components/layout/Header.jsx";
import MapCard from "../components/maps/MapCard.jsx";
import "./MyUserPage.css";
import { fetchMyMaps, getUsernameFromToken } from "../services/apiService.js";
import { useNavigate } from "react-router-dom"; // Importar hook

/* Hook de scroll horizontal*/
function useHorizontalScroll() {
  const ref = useRef(null);
  const api = useMemo(
    () => ({
      left: () => ref.current?.scrollBy({ left: -380, behavior: "smooth" }),
      right: () => ref.current?.scrollBy({ left: 380, behavior: "smooth" }),
    }),
    []
  );
  return { ref, ...api };
}

/* Cabecera simple*/
function SectionHeader({ title }) {
  return (
    <div className="section-header">
      <h3 className="section-title">{title}</h3>
    </div>
  );
}


/* Tarjeta simple de imagen */
function GalleryCard({ src, alt = "" }) {
  return (
    <article className="gallery-card">
      <img className="gallery-img" src={src} alt={alt} loading="lazy"
           onError={(e)=>{ e.currentTarget.style.display='none';
                           e.currentTarget.nextElementSibling.style.display='block'; }} />
    </article>
  );
}


const gallery = [
  { id: 1, url: "https://picsum.photos/seed/a/800/450" },
  { id: 2, url: "https://picsum.photos/seed/b/800/450" },
  { id: 3, url: "https://picsum.photos/seed/c/800/450" },
  { id: 4, url: "https://picsum.photos/seed/d/800/450" },
  { id: 5, url: "https://picsum.photos/seed/e/800/450" },
  { id: 6, url: "https://picsum.photos/seed/f/800/450" },
];

const user = { avatarUrl: "https://i.pravatar.cc/300?img=12" };

export default function MyUserPage() {
  const navigate = useNavigate();

  const [worlds, setWorlds] = useState([]); // Array vacío para los mundos reales
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  const username = getUsernameFromToken();
  const worldsScroll = useHorizontalScroll();
  const galleryScroll = useHorizontalScroll();

  useEffect(() => {
        const loadMyWorlds = async () => {
            if (!username) {
                // Si no hay usuario, mostramos un error o redirigimos
                setError("No se puede cargar la página sin iniciar sesión.");
                setIsLoading(false);
                return;
            }

            try {
                setIsLoading(true);
                setError(null);
                
                // 3. Llamada a la función de la API
                // NOTA: fetchMyMaps debería tomar el 'username' como argumento para filtrar en el backend
                const data = await fetchMyMaps(null); 
                
                setWorlds(data); // Almacena los mundos en el estado
            } catch (err) {
                console.error("Error al cargar mis mundos:", err);
                setError("Fallo al obtener tus mundos desde el servidor.");
                setWorlds([]);
            } finally {
                setIsLoading(false);
            }
        };

        loadMyWorlds();
    }, [username]);

  return (
    <div className="userpage">
      <Header />


        {/* Panel superior: Nombre de usuario, foto, editar perfil y acerca de mí.*/}
        <div className="top-panel">
            {/* Zona perfil*/}
            <div className="profile-box">
                {/* Recuadro perfil*/}
                <div className="profile-box">

                    {/* Foto de perfil */}
                    <div className="avatar">
                        <img src={user.avatarUrl} alt="Avatar" className="avatar-img"/>
                    </div>

                    {/* Nombre de usuario y botón editar perfil*/}
                    <div className="identity">
                        <h1 className="username">{username}</h1>
                        <button className="edit-btn" onClick={() => navigate("/edit-profile")}>
                            Editar perfil
                        </button>
                    </div>
                    
                </div>
            </div>

            {/* Zona "Acerca de mí" */}
            <div className="about-box">
                <div className="about-title">Acerca de mí:</div>
                <textarea className="about-textarea" placeholder="Escribe algo sobre ti..." />
            </div>

        </div>

        {/* Mis mundos */}
        <section className="section">
            <SectionHeader title="Mis mundos" />
            <div className="scroll-row show-scrollbar" ref={worldsScroll.ref}>
                {worlds.map((w) => (
                    <div key={w.id} className="card-wrap">
                        <MapCard mapData={w} />
                    </div>
                ))}
            </div>
        </section>

        {/* Galería */}
        <section className="section">
            <SectionHeader title="Galería" />
            <div className="scroll-row show-scrollbar" ref={galleryScroll.ref}>
                {gallery.map((g) => (
                <div key={g.id} className="card-wrap">
                    <GalleryCard src={g.url} alt={g.title} />
                </div>
                ))}
            </div>
        </section>
    </div>
  );
}

import React, { useState, useEffect, useMemo, useRef } from "react";
import { useParams } from "react-router-dom";
import Header from "../components/layout/Header.jsx";
import MapCard from "../components/maps/MapCard.jsx";
import "./MyUserPage.css";
import { fetchUserByUsername, fetchUserMaps } from "../services/apiService.js";

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

/* Datos de ejemplo: sustituir por API */
const worlds = [
  { id: 1, title: "Survival", version: "1.12.2", type: "Java", description: "" },
  { id: 2, title: "Servidor amigos", version: "1.19.2", type: "Bedrock", description: "" },
  { id: 3, title: "Karmaland", version: "1.14.1", type: "Java", description: "MODS" },
  { id: 4, title: "Morbiland", version: "1.19.2", type: "Java", description: "" },
  { id: 5, title: "Hardcore", version: "1.21.6", type: "Java", description: "Priv." },
  { id: 6, title: "Survival", version: "1.12.2", type: "Java", description: "" },
  { id: 7, title: "Servidor amigos", version: "1.19.2", type: "Bedrock", description: "" },
  { id: 8, title: "Karmaland", version: "1.14.1", type: "Java", description: "MODS" },
  { id: 9, title: "Morbiland", version: "1.19.2", type: "Java", description: "" },
  { id: 10, title: "Hardcore", version: "1.21.6", type: "Java", description: "Priv." },
];

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

  const { username } = useParams();

  const [user, setUser] = useState(null);
  const [worlds, setWorlds] = useState([]);

  useEffect(() => {
        const loadProfile = async () => {

            try {
                const data = await fetchUserByUsername(username);
                setUser(data);
                
            } catch (error) {
                console.error(`Error cargando el perfil de ${username}:`, error);
                setUser(null); // Asegura que el perfil esté vacío si falla
            }
            try {
                const mapas = await fetchUserMaps(username, null)
                setWorlds(mapas)
            } catch (err) {
                console.error(`Error cargando los mundos de ${username}:`, error);
                setWorlds(null); 
            }
        };

        loadProfile();
    }, [username]);

    const worldsScroll = useHorizontalScroll();
    const galleryScroll = useHorizontalScroll();


    if (!user) {
        return (
            <div className="userpage">
                <Header />
                <p className="loading-message">Cargando perfil o buscando usuario...</p>
            </div>
        );
    }


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
                        <h1 className="username">{user.username}</h1>
                    </div>
                    
                </div>
            </div>

            {/* Zona "Acerca de mí" */}
            <div className="about-box">
                <div className="about-title">Acerca de mí:</div>
                <p className="about-textarea" >{user.description}</p>
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

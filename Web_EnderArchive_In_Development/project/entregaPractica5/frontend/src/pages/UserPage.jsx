import React, { useState, useEffect, useMemo, useRef } from "react";
import { useParams } from "react-router-dom";
import Header from "../components/layout/Header.jsx";
import MapCard from "../components/maps/MapCard.jsx";
import "./MyUserPage.css";
// 1. IMPORTAR FUNCIONES
import {
  fetchUserByUsername,
  fetchUserMaps,
  fetchWorldImages,
  fetchImageForDisplay,
} from "../services/apiService.js";

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

function SectionHeader({ title }) {
  return (
    <div className="section-header">
      <h3 className="section-title">{title}</h3>
    </div>
  );
}

function GalleryCard({ src, alt = "" }) {
  return (
    <article className="gallery-card">
      <img
        className="gallery-img"
        src={src}
        alt={alt}
        loading="lazy"
        onError={(e) => {
          e.currentTarget.style.display = "none";
        }}
      />
    </article>
  );
}

export default function UserPage() {
  const { username } = useParams();

  const [user, setUser] = useState(null);
  const [worlds, setWorlds] = useState([]);
  const [gallery, setGallery] = useState([]); // Estado Galería

  const worldsScroll = useHorizontalScroll();
  const galleryScroll = useHorizontalScroll();

  useEffect(() => {
    const loadProfile = async () => {
      // A. Cargar Usuario
      try {
        const data = await fetchUserByUsername(username);
        // Aseguramos estructura de datos por si viene algo null
        setUser(data);
      } catch (error) {
        console.error(`Error cargando el perfil de ${username}:`, error);
        setUser(null);
      }

      // B. Cargar Mundos y luego Galería
      try {
        const mapas = await fetchUserMaps(username, null);
        setWorlds(mapas);

        // --- C. LOGICA DE GALERÍA ---
        // Filtramos mundos públicos (aunque fetchUserMaps ya debería devolver solo públicos si no eres tú, aseguramos)
        const publicWorlds = mapas.filter((w) => w.public === true);
        const allImages = [];

        await Promise.all(
          publicWorlds.map(async (world) => {
            try {
              const imagesMeta = await fetchWorldImages(
                world.id.worldOwner,
                world.id.worldName
              );

              if (imagesMeta && imagesMeta.length > 0) {
                const processedImages = await Promise.all(
                  imagesMeta.map(async (img) => {
                    const fullPath = img.id?.filePath || "";
                    const fileName = fullPath.split("/").pop();

                    if (!fileName) return null;

                    const urlApi = `http://localhost:3000/uploads/${world.id.worldOwner}/worlds/${world.id.worldName}/photos/${fileName}`;
                    const blobUrl = await fetchImageForDisplay(urlApi);

                    if (blobUrl) {
                      return {
                        url: blobUrl,
                        title: world.displayName,
                      };
                    }
                    return null;
                  })
                );
                allImages.push(...processedImages.filter((i) => i !== null));
              }
            } catch (err) {
              // Ignorar errores puntuales de imágenes
            }
          })
        );

        setGallery(allImages);
      } catch (err) {
        console.error(
          `Error cargando mundos/galería de ${user.username}:`,
          err
        );
        setWorlds([]);
      }
    };

    if (username) loadProfile();
  }, [username]);

  if (!user) {
    return (
      <div className="userpage">
        <Header />
        <p
          className="loading-message"
          style={{ color: "white", padding: "20px" }}
        >
          Cargando perfil...
        </p>
      </div>
    );
  }

  return (
    <div className="userpage">
      <Header />

      <div className="top-panel">
        <div className="profile-box">
          <div className="profile-box">
            <div className="avatar">
              <img src={user.profilePic} alt="Avatar" className="avatar-img" />
            </div>
            <div className="identity">
              <h1 className="username">{user.username}</h1>
            </div>
          </div>
        </div>

        <div className="about-box">
          <div className="about-title">Acerca de mí:</div>
          <p className="about-textarea">
            {user.description || "Sin descripción disponible."}
          </p>
        </div>
      </div>

      {/* Mis mundos */}
      <section className="section">
        <SectionHeader title={`Mundos de ${user.username}`} />
        <div className="scroll-row show-scrollbar" ref={worldsScroll.ref}>
          {worlds.length > 0 ? (
            worlds.map((w) => (
              <div key={w.id.worldName} className="card-wrap">
                <MapCard mapData={w} />
              </div>
            ))
          ) : (
            <p className="error-text">Este usuario no tiene mundos públicos.</p>
          )}
        </div>
      </section>

      {/* Galería */}
      <section className="section">
        <SectionHeader title="Galería" />
        <div className="scroll-row show-scrollbar" ref={galleryScroll.ref}>
          {gallery.length > 0 ? (
            gallery.map((g, index) => (
              <div key={index} className="card-wrap">
                <GalleryCard src={g.url} alt={g.title} />
              </div>
            ))
          ) : (
            <p
              className="error-text"
              style={{ color: "#888", fontStyle: "italic" }}
            >
              No hay imágenes para mostrar.
            </p>
          )}
        </div>
      </section>
    </div>
  );
}

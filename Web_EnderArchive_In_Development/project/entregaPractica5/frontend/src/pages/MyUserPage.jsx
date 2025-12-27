import React, { useState, useEffect, useMemo, useRef } from "react";
import Header from "../components/layout/Header.jsx";
import MapCard from "../components/maps/MapCard.jsx";
import "./MyUserPage.css";
import {
  fetchMyMaps,
  getUsernameFromToken,
  fetchWorldImages,
  fetchImageForDisplay,
  fetchUserByUsername,
} from "../services/apiService.js";
import { useNavigate } from "react-router-dom";

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
      <img
        className="gallery-img"
        src={src}
        alt={alt}
        loading="lazy"
        onError={(e) => {
          e.currentTarget.style.display = "none";
          // e.currentTarget.nextElementSibling.style.display = "block"; // Asegúrate de tener un fallback si usas esto
        }}
      />
    </article>
  );
}

export default function MyUserPage() {
  const navigate = useNavigate();

  const [worlds, setWorlds] = useState([]);
  const [gallery, setGallery] = useState([]); 
  const [userData, setUserData] = useState({ description: "", avatarUrl: "" }); 
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);

  const username = getUsernameFromToken();
  const worldsScroll = useHorizontalScroll();
  const galleryScroll = useHorizontalScroll();

  useEffect(() => {
    const loadPageData = async () => {
      if (!username) {
        setError("No se puede cargar la página sin iniciar sesión.");
        setIsLoading(false);
        return;
      }

      try {
        setIsLoading(true);
        setError(null);

        // --- A. CARGAR DATOS DEL USUARIO ---
        try {
          const userDetails = await fetchUserByUsername(username);
          setUserData({
            description: userDetails.description || "Sin descripción.",
            avatarUrl:
              userDetails.profilePic || "https://i.pravatar.cc/300?img=12",
          });
        } catch (e) {
          console.warn("No se pudo cargar detalles del usuario", e);
        }

        // --- B. CARGAR MUNDOS ---
        // fetchMyMaps devuelve TODOS los mapas del usuario (públicos y privados)
        const data = await fetchMyMaps(null);
        setWorlds(data);

        // --- C. CARGAR GALERÍA (Públicos y Privados) ---
        const allImages = [];

        // CORRECCIÓN CLAVE: Iteramos sobre 'data' (variable local con los datos reales),
        // no sobre 'worlds' (estado que aún no se ha actualizado en este ciclo).
        await Promise.all(
          data.map(async (world) => {
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
                        id: img.id.filePath,
                        url: blobUrl,
                        title: world.displayName,
                      };
                    }
                    return null;
                  })
                );

                // Añadimos las válidas al array general
                allImages.push(...processedImages.filter((i) => i !== null));
              }
            } catch (err) {
              // Ignoramos errores puntuales de imágenes para no romper toda la página
              console.warn(
                `Error cargando imágenes del mundo ${world.displayName}`,
                err
              );
            }
          })
        );

        setGallery(allImages);
      } catch (err) {
        console.error("Error general:", err);
        setError("Fallo al obtener datos.");
      } finally {
        setIsLoading(false);
      }
    };

    loadPageData();
  }, [username]);

  return (
    <div className="userpage">
      <Header />

      <div className="top-panel">
        <div className="profile-box">
          <div className="profile-box">
            <div className="avatar">
              <img
                src={userData.avatarUrl}
                alt="Avatar"
                className="avatar-img"
              />
            </div>
            <div className="identity">
              <h1 className="username">{username}</h1>
              <button
                className="edit-btn"
                onClick={() => navigate("/edit-profile")}
              >
                Editar perfil
              </button>
            </div>
          </div>
        </div>

        <div className="about-box">
          <div className="about-title">Acerca de mí:</div>
          <textarea
            className="about-textarea"
            value={userData.description}
            readOnly
          />
        </div>
      </div>

      {/* Mis mundos */}
      <section className="section">
        <SectionHeader title="Mis mundos" />
        <div className="scroll-row show-scrollbar" ref={worldsScroll.ref}>
          {worlds.length > 0 ? (
            worlds.map((w) => (
              <div key={w.displayName + w.id.worldName} className="card-wrap">
                <MapCard mapData={w} />
              </div>
            ))
          ) : (
            <p className="error-text">No hay mundos creados.</p>
          )}
        </div>
      </section>

      {/* Galería Dinámica */}
      <section className="section">
        <SectionHeader title="Galería (Todos mis mundos)" />
        <p style={{ fontSize: "0.9em", color: "#aaa", marginBottom: "10px" }}>
          (Tus fotos privadas solo las ves tú. Los demás verán solo las públicas).
        </p>
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
              No hay imágenes en tus mundos.
            </p>
          )}
        </div>
      </section>
    </div>
  );
}
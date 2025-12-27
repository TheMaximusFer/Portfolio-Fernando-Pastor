import React, { useEffect, useState } from "react";
import { useParams, useNavigate } from "react-router-dom";
import Header from "../components/layout/Header.jsx";
import "./WorldDetail.css";
import ReportWindow from "./ReportWindow.jsx";
import NavigationBar from '../components/layout/NavigationBar.jsx';
import {
  fetchUserMaps,
  fetchMarkers,
  postReport,
  postFavourite,
  fetchAllMyFavourites,
  deleteFavourite,
  downloadFile,
  fetchWorldImages,
  fetchImageForDisplay,
} from "../services/apiService.js";
import SanctionWindow from "./SanctionWindow.jsx";
// src/pages/WorldDetail.jsx

const ArrowLeft = () => (
  <svg viewBox="0 0 24 24" width="18" height="18" aria-hidden="true">
    <path
      fill="currentColor"
      d="M15.41 7.41 14 6 8 12l6 6 1.41-1.41L10.83 12z"
    />
  </svg>
);
const ArrowRight = () => (
  <svg viewBox="0 0 24 24" width="18" height="18" aria-hidden="true">
    <path
      fill="currentColor"
      d="M8.59 16.59 10 18l6-6-6-6-1.41 1.41L13.17 12z"
    />
  </svg>
);

const StarIcon = ({ filled }) => (
  <svg
    viewBox="0 0 24 24"
    width="24"
    height="24"
    stroke="currentColor"
    strokeWidth="2"
    fill={filled ? "currentColor" : "none"}
    strokeLinecap="round"
    strokeLinejoin="round"
  >
    <polygon points="12 2 15.09 8.26 22 9.27 17 14.14 18.18 21.02 12 17.77 5.82 21.02 7 14.14 2 9.27 8.91 8.26 12 2" />
  </svg>
);

export default function WorldDetail() {
  const navigate = useNavigate();
  const { worldOwner, worldName } = useParams();

  const [worldData, setWorldData] = useState(null);
  const [markers, setMarkersData] = useState([]);
  const [isLoading, setIsLoading] = useState(true);
  const [error, setError] = useState(null);
  const [isFavorite, setIsFavorite] = useState(false);
  const [images, setImages] = useState(null);

  useEffect(() => {
    const loadWorldDetails = async () => {
      setIsLoading(true);
      setError(null);

      try {
        const worldResult = await fetchUserMaps(worldOwner, worldName);
        setWorldData(worldResult);
        console.log(`el mundo es: ${worldResult.displayName}`)
        const markersResult = await fetchMarkers(worldOwner, worldName);
        const imagesList = await fetchWorldImages(worldOwner, worldName);

        const worldImagesBlobUrls = await Promise.all(
          imagesList.map(async (imgObj) => {
            const fullPath = imgObj.id?.filePath || "";
            const fileName = fullPath.split("/").pop();

            if (!fileName) return null;

            const urlApi = `http://localhost:3000/uploads/${worldOwner}/worlds/${worldName}/photos/${fileName}`;

            return await fetchImageForDisplay(urlApi);
          })
        );
        // Filtramos los nulos (por si alguna falló)
        const validWorldImagesUrls = worldImagesBlobUrls.filter((url) => url !== null);
        setImages(validWorldImagesUrls);

        var finalMarkerImages
        var cont = 0
        for (const m of markersResult) {
          const fullPath = m.imagePath || "";
          const fileName = fullPath.split("/").pop();
          const urlApi = `http://localhost:3000/uploads/${worldOwner}/worlds/${worldName}/markers/${m.id.idMarker}/${fileName}`

          if (!fileName) continue;

          m.photo = await fetchImageForDisplay(urlApi)
          cont++
        }

        setMarkersData(markersResult);
        
        

        const token = localStorage.getItem("jwtToken");
        if (token) {
          var favourites = await fetchAllMyFavourites();

          const existe = favourites.some((item) => {
            // Aseguramos que existan los datos antes de comparar
            const itemName = item.id?.worldName || "";
            const itemOwner = item.id?.worldOwner || "";

            // Comparamos todo en minúsculas para evitar errores de URL
            return (
              itemName.toLowerCase() === worldName.toLowerCase() &&
              itemOwner === worldOwner
            );
          });
          console.log("¿Encontrado?:", existe);
          setIsFavorite(existe);
        }
      } catch (e) {
        console.error("Fallo al cargar los detalles del mundo:", e);
        setError("No se pudieron cargar los datos del mundo o los marcadores.");
      } finally {
        setIsLoading(false);
      }
    };

    loadWorldDetails();
  }, [worldOwner, worldName]);

  const handleDownload = () => {
    if (worldData.filePath != null)
      downloadFile(worldData.filePath, "world.zip");
  };

  //Para reportar
  const [openReport, setOpenReport] = useState(false);
  const username = worldOwner;

  const markFavourite = () => {
    if (isFavorite) {
      setIsFavorite(false);
      deleteFavourite(worldOwner, worldName);
    } else {
      setIsFavorite(true);
      postFavourite(worldOwner, worldName);
    }
  };

  //Para el sistema de botones e imágenes
  const [id, setId] = useState(0); //Estado de imagen seleccionada
  const prev = () => setId((i) => (i - 1 + images.length) % images.length);
  const next = () => setId((i) => (i + 1) % images.length);

  if (isLoading) {
    return (
      <div className="loading-page">
        <Header />
        <p>
          Cargando detalles de **{worldName}** de **{worldOwner}**...
        </p>
      </div>
    );
  }

  if (error) {
    return <div className="error-page">{error}</div>;
  }

  if (!worldData) {
    console.log(worldData)
    return (
      <div className="error-page">Mundo no encontrado o datos incompletos.</div>
    );
  }

  var tags = [];
  tags.push(worldData.version);
  tags.push(worldData.platform);

  if (worldData.adventure == true) {
    tags.push("Adventure");
  }
  if (worldData.survival == true) {
    tags.push("Survival");
  }
  if (worldData.fantasy == true) {
    tags.push("Fantasy");
  }
  if (worldData.skyblock == true) {
    tags.push("Skyblock");
  }

  return (
    <div className="worldpage">
      <Header />
      <NavigationBar />
      <div className="detail-shell">
        <div className="window-grid">
          {/* COLUMNA IZQUIERDA: título + descripción + marcadores */}
          <section className="window-left">
            {/* Título y autor */}
            <header className="window-head">
              <h1 className="window-title">
                <span className="underline">{worldData.displayName}</span>{" "}
                {/* --- CAMBIO AQUÍ --- */}
                <span className="by">
                  POR:{" "}
                  <span
                    className="author-link"
                    onClick={() =>
                      navigate(`/user-profile/${worldData.id.worldOwner}`)
                    }
                    title="Ver perfil del creador"
                  >
                    {worldData.id.worldOwner}
                  </span>
                </span>
                {/* ------------------- */}
                <button
                  className={`star-btn ${isFavorite ? "active" : ""}`}
                  onClick={() => markFavourite()}
                  aria-label="Añadir a favoritos"
                >
                  <StarIcon filled={isFavorite} />
                </button>
              </h1>
            </header>

            {/* Descripción */}
            <div className="desc-box">{worldData.description}</div>

            {/* Marcadores */}
            {/* Título de tags */}
            <h3 className="section-title">MARCADORES</h3>
            {/* Panel de tags */}
            <div className="markers-pane">
              {/* Lista de tags con su formato */}
              {markers.map((m) => (
                <article key={m.id} className="marker-row">
                  {/* Imagen del marcador */}
                  <div className="thumb">
                    <img className="tag-img" src={m.photo} />
                  </div>
                  {/* Nombre del marcador */}
                  <div className="marker-main">
                    <div className="marker-name">{m.id.idMarker}</div>
                  </div>
                  {/* Dimensión */}
                  <div className="dimension">
                    <div className="dimension-value">{m.dimension}</div>
                  </div>
                  {/* Recuadro de coordenadas */}
                  <div className="coords">
                    <div className="coords-title">COORDENADAS</div>
                    <div className="coords-line">
                      <span>X</span> {m.x}
                    </div>
                    <div className="coords-line">
                      <span>Y</span> {m.y}
                    </div>
                    <div className="coords-line">
                      <span>Z</span> {m.z}
                    </div>
                  </div>
                </article>
              ))}
            </div>
          </section>

          {/* COLUMNA DERECHA: galería + puntos + tags + descargar */}
          <aside className="window-right">
            {/* Galería */}
            {/* Título de galería (y simbolo de reportar) */}
            <div className="right-header">
              <h3 className="section-title">GALERÍA</h3>
              <div className="right-header">
                <button className="warn" onClick={() => setOpenReport(true)}>
                  ⚠
                </button>
              </div>
            </div>

            {/* Fotos */}
            <div className="gallery-box">
              {images && images.length > 0 ? (
                <img
                  className="gallery-img"
                  src={images[id]}
                  alt={`Captura ${id + 1}`}
                />
              ) : (
                <div className="no-images-placeholder">
                  <p>Sin imágenes disponibles</p>
                </div>
              )}
            </div>

            {/* Control de la galería */}
            {images && images.length > 0 && (
              <div className="gallery-controls">
                {/* Botón izquierda */}
                <button
                  className="square-btn"
                  onClick={prev}
                  aria-label="Anterior"
                >
                  <ArrowLeft />
                </button>
                {/* Tantos botones como imágenes tenga el mundo */}
                <div className="dots">
                  {images.map((_, i) => (
                    <button
                      key={i}
                      className={`dot ${i === id ? "active" : ""}`}
                      onClick={() => setId(i)}
                      aria-label={`Ir a la imagen ${i + 1}`}
                    />
                  ))}
                </div>
                {/* Botón derecha */}
                <button
                  className="square-btn"
                  onClick={next}
                  aria-label="Siguiente"
                >
                  <ArrowRight />
                </button>
              </div>
            )}

            {/* Zona de tags */}
            <div className="tags-box">
              {tags.map((t) => (
                <span key={t} className="tag-chip">
                  #{t}
                </span>
              ))}
            </div>

            {worldData.filePath ? (
              <button className="download-btn" onClick={handleDownload}>
                DESCARGAR
              </button>
            ) : (
              <div 
                style={{ 
                  marginTop: "20px", 
                  textAlign: "center", 
                  color: "#aaa", 
                  fontStyle: "italic",
                  fontSize: "0.9rem" 
                }}
              >
                No hay fichero asociado al mundo
              </div>
            )}
          </aside>
        </div>

        <ReportWindow
          isOpen={openReport}
          username={username}
          onClose={() => setOpenReport(false)}
          onConfirm={(data) => {
            postReport(data);
            console.log("Denuncia enviada:", data);
            setOpenReport(false);
          }}
          primaryLabel="Denunciar"
        />
      </div>
    </div>
  );
}

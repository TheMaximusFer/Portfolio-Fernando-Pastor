import React, { useEffect, useRef, useState } from "react";
import Header from "../components/layout/Header.jsx";
import "./WorldCreate.css";
import { useNavigate } from "react-router-dom";
import {
  postWorld,
  postMarker,
  getUsernameFromToken,
  postWorldImage,
  postMarkerImage,
  postWorldFile,
} from "../services/apiService.js";
import NavigationBar from "../components/layout/NavigationBar.jsx";

/* Catálogo del sistema: solo se pueden añadir estas tags */
const ALL_TAGS = ["survival", "adventure", "fantasy", "skyblock"];

const ALL_VERSIONS = ["1.20", "1.19", "1.18", "1.8"];

export default function WorldCreate() {
  const navigate = useNavigate();

  
  /* --------- Formulario básico (sin contenido de ejemplo) --------- */
  const [worldName, setWorldName] = useState("");
  const [mcVersion, setMcVersion] = useState("");
  const [description, setDescription] = useState("");

  /* --------- TAGS (solo del catálogo) --------- */
  const [selectedTags, setSelectedTags] = useState([]);
  const [search, setSearch] = useState("");
  const [showSuggest, setShowSuggest] = useState(false);

  const [worldFile, setWorldFile] = useState(null);

  const filteredSuggestions = ALL_TAGS.filter(
    (t) =>
      t.toLowerCase().includes(search.trim().toLowerCase()) &&
      !selectedTags.includes(t)
  ).slice(0, 8);

  const addTag = (tag) => {
    const hit = ALL_TAGS.find((t) => t.toLowerCase() === tag.toLowerCase());
    if (!hit) return;
    if (selectedTags.includes(hit)) return;
    setSelectedTags((prev) => [...prev, hit]);
    setSearch("");
    setShowSuggest(false);
  };

  const removeTag = (tag) => {
    setSelectedTags((prev) => prev.filter((t) => t !== tag));
  };

  const onSearchKeyDown = (e) => {
    if (e.key === "Enter") {
      e.preventDefault();
      if (filteredSuggestions.length > 0) {
        addTag(filteredSuggestions[0]);
      }
    } else if (e.key === "Escape") {
      setShowSuggest(false);
    }
  };

  /* --------- Marcadores (vacío inicialmente) --------- */
  const newMarker = () => ({
    id: crypto.randomUUID(),
    nombre: "",
    x: "",
    y: "",
    z: "",
    photoUrl: "",
    dimension: "Overworld", // Esta es la propiedad nueva
  });

  const [markers, setMarkers] = useState([]);

  const addMarker = () => setMarkers((p) => [...p, newMarker()]);
  const updateMarker = (idx, field, value) => {
    setMarkers((prev) => {
      const next = [...prev];
      next[idx] = { ...next[idx], [field]: value };
      return next;
    });
  };
  const removeMarkerRow = (idx) => {
    setMarkers((prev) => {
      const victim = prev[idx];
      if (victim?.photoUrl?.startsWith("blob:"))
        URL.revokeObjectURL(victim.photoUrl);
      const next = [...prev];
      next.splice(idx, 1);
      return next;
    });
  };

  const handleMarkerPhotoSelect = (idx, files) => {
    const file = files?.[0];
    if (!file) return;

    setMarkers((prev) => {
      const next = [...prev];
      const old = next[idx];

      // Limpiamos memoria de la foto anterior si existía
      if (old.photoUrl?.startsWith("blob:")) {
        URL.revokeObjectURL(old.photoUrl);
      }

      // CAMBIO: Ahora guardamos 'file' (el archivo) Y 'photoUrl' (la vista previa)
      next[idx] = { 
        ...old, 
        file: file,                  // <--- El fichero real
        photoUrl: URL.createObjectURL(file) // <--- La vista previa
      };
      
      return next;
    });
  };

  const onMarkerDrop = (idx, e) => {
    e.preventDefault();
    handleMarkerPhotoSelect(idx, e.dataTransfer.files);
  };

  /* --------- Galería derecha (vacía) --------- */
  const [thumbs, setThumbs] = useState([]);
  const galleryInputRef = useRef(null);

  const addToGallery = (list) => {
    const files = Array.from(list || []);
    if (!files.length) return;

    setThumbs((prev) => [
      ...prev,
      ...files.map((f) => ({
        id: crypto.randomUUID(),
        file: f, // <--- AQUÍ GUARDAMOS EL FICHERO REAL
        previewUrl: URL.createObjectURL(f), // Usamos esto solo para verla en pantalla
      })),
    ]);
  };

  const delThumb = (id) => {
    setThumbs((prev) => {
      const t = prev.find((x) => x.id === id);
      // Limpiamos memoria
      if (t?.previewUrl?.startsWith("blob:")) URL.revokeObjectURL(t.previewUrl);
      return prev.filter((x) => x.id !== id);
    });
  };

  /* --------- Carpeta del mundo (ruta visible) --------- */
  const folderInputRef = useRef(null);
  const [folderDisplay, setFolderDisplay] = useState("");

  // Plataforma
  const [platform, setPlatform] = useState("Java"); // por defecto Java

  const [isPublic, setIsPublic] = useState(false);
  const [message, setMessage] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false);

  const onPickFolder = (e) => {
    const file = e.target.files?.[0];
    
    if (!file) {
      setFolderDisplay("");
      setWorldFile(null);
      return;
    }

    // Guardamos el fichero real para el submit
    setWorldFile(file);
    
    // Mostramos el nombre en pantalla
    setFolderDisplay(file.name);
  };

  // Usuario sacado del token JWT
  let user = null;
  try {
    user = getUsernameFromToken();
  } catch (e) {
    console.error("Error obteniendo usuario desde el token", e);
  }

  /* --------- limpiar blobs al desmontar --------- */
  useEffect(() => {
    const token = localStorage.getItem('jwtToken');
    if (!token) {
      navigate('/login'); // Redirige inmediatamente si no hay sesión
    }
    return () => {
      markers.forEach(
        (m) =>
          m.photoUrl?.startsWith("blob:") && URL.revokeObjectURL(m.photoUrl)
      );
      thumbs.forEach(
        (t) => t.previewUrl?.startsWith("blob:") && URL.revokeObjectURL(t.url)
      );
    };
  }, [navigate]); // solo al desmontar

  const handleSubmit = async () => {
    if (!user) {
      setMessage("Error: no se detecta usuario logueado (token inválido).");
      return;
    }

    if (!worldName || !mcVersion) {
      setMessage("Nombre y versión son obligatorios.");
      return;
    }

    setIsSubmitting(true);
    setMessage("Creando mundo...");

    // DTO del mundo para el backend
    const worldDTO = {
      id: { worldName: worldName },
      description: description,
      platform: platform,
      version: mcVersion,
      public: isPublic,
    };

    // Tags como booleanos: survival=true, etc.
    ALL_TAGS.forEach((tag) => {
      worldDTO[tag.toLowerCase()] = null;
    });

    selectedTags.forEach((tag) => {
      worldDTO[tag.toLowerCase()] = true;
    });

    try {
      // 1) Crear el mundo
      await postWorld(worldDTO);

      if (worldFile) {
        setMessage("Subiendo archivo del mundo (.zip)...");
        postWorldFile(worldName, worldFile)
      }

      // 2) Crear marcadores (si hay)
      if (markers.length > 0) {
        setMessage(`Subiendo ${markers.length} marcadores...`);

        for (const m of markers) {
          if (!m.nombre) continue; // saltamos marcadores sin nombre

          const markerDTO = {
            id: { idMarker: m.nombre },
            x: parseInt(m.x || 0),
            y: parseInt(m.y || 0),
            z: parseInt(m.z || 0),
            dimension: m.dimension, // usamos la dimensión seleccionada en los botones
          };

          // Igual que en WC.jsx: pasamos usuario y nombre del mundo
          await postMarker(worldName, markerDTO);

          if (m.file) {

            postMarkerImage(m.nombre, worldName, m.file)

          }
        }
      }

      for (const t of thumbs) {
        postWorldImage(worldName, t.file);
      }
      console.log("Mundo subido finalmente")

      setMessage("¡Mundo creado! Redirigiendo...");
      setTimeout(() => navigate("/"), 1500);
    } catch (error) {
      console.error(error);
      setMessage(`Error: ${error.message}`);
    } finally {
      setIsSubmitting(false);
    }
  };

  return (
    <div className="uploadpage">
      <Header />
      <NavigationBar />
      <div className="form-shell">
        <div className="form-titlee-row">
          <h1 className="form-titlee">SUBIR MUNDO</h1>
        </div>

        <div className="form-grid">
          {/* -------------------- IZQUIERDA -------------------- */}
          <div className="col-left">
            {/* Nombre + versión */}
            <div className="row">
              <label className="label-block">NOMBRE</label>
              <div className="name-line">
                <input
                  className="input"
                  value={worldName}
                  onChange={(e) => setWorldName(e.target.value)}
                  placeholder="Nombre del mundo"
                />
                <select
                  className="select"
                  value={mcVersion}
                  onChange={(e) => setMcVersion(e.target.value)}
                >
                  <option value="" disabled>
                    Selecciona versión
                  </option>
                  {ALL_VERSIONS.filter((t) => /\d/.test(t)).map((v) => (
                    <option key={v} value={v}>
                      {v}
                    </option>
                  ))}
                </select>
              </div>
            </div>
            {/* Botones java-bedrock */}
            <div className="toggle-group">
              {/* Toggle Java / Bedrock (el tuyo, a la derecha) */}
              <div
                className="platform-toggle"
                role="tablist"
                aria-label="Plataforma"
              >
                <button
                  type="button"
                  className={`plat-btn ${platform === "Java" ? "active" : ""}`}
                  onClick={() => setPlatform("Java")}
                  aria-pressed={platform === "Java"}
                >
                  Java
                </button>
                <button
                  type="button"
                  className={`plat-btn ${
                    platform === "Bedrock" ? "active" : ""
                  }`}
                  onClick={() => setPlatform("Bedrock")}
                  aria-pressed={platform === "Bedrock"}
                >
                  Bedrock
                </button>
              </div>
            </div>

            {/* Descripción */}
            <div className="row">
              <label className="label-block">DESCRIPCIÓN</label>
              <textarea
                className="textarea"
                rows={6}
                value={description}
                onChange={(e) => setDescription(e.target.value)}
                placeholder="Describe tu mundo…"
              />
            </div>

            {/* Tags del mundo */}
            <div className="row">
              <label className="label-block">TAGS DEL MUNDO</label>

              <div className="tag-search-wrap">
                <input
                  className="input tag-search"
                  placeholder="Buscar tag del sistema…"
                  value={search}
                  onChange={(e) => {
                    setSearch(e.target.value);
                    setShowSuggest(true);
                  }}
                  onKeyDown={onSearchKeyDown}
                  onFocus={() => setShowSuggest(true)}
                />

                {showSuggest && filteredSuggestions.length > 0 && (
                  <ul
                    className="suggest-list"
                    onMouseLeave={() => setShowSuggest(false)}
                  >
                    {filteredSuggestions.map((s) => (
                      <li
                        key={s}
                        className="suggest-item"
                        onMouseDown={(e) => {
                          e.preventDefault();
                          addTag(s);
                        }}
                      >
                        {s}
                      </li>
                    ))}
                  </ul>
                )}
              </div>

              <div className="tags-boxx">
                {selectedTags.map((t) => (
                  <span key={t} className="tag-chip">
                    #{t}
                    <button
                      type="button"
                      className="chip-x"
                      aria-label={`Quitar ${t}`}
                      onClick={() => removeTag(t)}
                    >
                      ×
                    </button>
                  </span>
                ))}
              </div>
            </div>

            {/* Marcadores */}
            <div className="row row--grow">
              <label className="label-block">MARCADORES</label>
              <div className="markers-panel">
                {markers.map((m, i) => (
                  <article key={m.id} className="marker-card">
                    {/* Upload + preview */}
                    <div
                      className="marker-upload"
                      onDrop={(e) => onMarkerDrop(i, e)}
                      onDragOver={(e) => e.preventDefault()}
                    >
                      <input
                        id={`mphoto-${m.id}`}
                        type="file"
                        accept="image/*"
                        className="marker-photo-input"
                        onChange={(e) =>
                          handleMarkerPhotoSelect(i, e.target.files)
                        }
                      />
                      <label
                        htmlFor={`mphoto-${m.id}`}
                        className={`marker-upload-box ${
                          m.photoUrl ? "has-image" : ""
                        }`}
                        title="Subir imagen del marcador (clic o arrastra aquí)"
                      >
                        {m.photoUrl ? (
                          <img
                            src={m.photoUrl}
                            alt="Imagen del marcador"
                            className="marker-upload-img"
                          />
                        ) : (
                          <span className="upload-arrow">↑</span>
                        )}
                      </label>
                    </div>

                    {/* Nombre */}
                    <div>
                      <div className="marker-main">
                        <div style={{ width: "100%" }}>
                          <span className="mini-label">
                            NOMBRE DEL MARCADOR
                          </span>
                          <div className="fieldline">
                            <input
                              className="input"
                              value={m.nombre}
                              onChange={(e) =>
                                updateMarker(i, "nombre", e.target.value)
                              }
                              placeholder="Nombre del marcador"
                            />
                          </div>
                        </div>
                      </div>

                      {/* Dimensión */}
                      <div
                        className="dimension-toggle"
                        role="tablist"
                        aria-label="Dimensión"
                      >
                        <button
                          type="button"
                          className={`plat-btn ${
                            m.dimension === "Overworld" ? "active" : ""
                          }`}
                          onClick={() =>
                            updateMarker(i, "dimension", "Overworld")
                          }
                          aria-pressed={m.dimension === "Overworld"}
                        >
                          Overworld
                        </button>

                        <button
                          type="button"
                          className={`plat-btn ${
                            m.dimension === "Nether" ? "active" : ""
                          }`}
                          onClick={() => updateMarker(i, "dimension", "Nether")}
                          aria-pressed={m.dimension === "Nether"}
                        >
                          Nether
                        </button>

                        <button
                          type="button"
                          className={`plat-btn ${
                            m.dimension === "End" ? "active" : ""
                          }`}
                          onClick={() => updateMarker(i, "dimension", "End")}
                          aria-pressed={m.dimension === "End"}
                        >
                          End
                        </button>
                      </div>
                    </div>

                    {/* Coordenadas */}
                    <div className="marker-coords">
                      <h4 className="coords-title">COORDENADAS</h4>
                      <div className="coords-line">
                        <span className="coord-label">X</span>
                        <input
                          className="coord-input"
                          value={m.x}
                          onChange={(e) => updateMarker(i, "x", e.target.value)}
                          placeholder="—"
                          type="number"
                        />
                        <span className="coord-label">Y</span>
                        <input
                          className="coord-input"
                          value={m.y}
                          onChange={(e) => updateMarker(i, "y", e.target.value)}
                          placeholder="—"
                          type="number"
                        />
                        <span className="coord-label">Z</span>
                        <input
                          className="coord-input"
                          value={m.z}
                          onChange={(e) => updateMarker(i, "z", e.target.value)}
                          placeholder="—"
                          type="number"
                        />
                      </div>
                    </div>

                    {/* Eliminar */}
                    <button
                      className="circle-btn"
                      type="button"
                      aria-label="Eliminar marcador"
                      onClick={() => removeMarkerRow(i)}
                    />
                  </article>
                ))}

                {/* Añadir */}
                <button
                  className="marker-add"
                  type="button"
                  onClick={addMarker}
                >
                  <span className="plus">+</span>
                </button>
              </div>
            </div>
          </div>

          {/* -------------------- DERECHA -------------------- */}
          <div className="col-right">
            <div className="gallery-stack">
              {/* Subida de imágenes a la galería */}
              <div
                className="gallery-drop"
                onDragOver={(e) => e.preventDefault()}
                onDrop={(e) => {
                  e.preventDefault();
                  addToGallery(e.dataTransfer.files);
                }}
                onClick={() => galleryInputRef.current?.click()}
                title="Arrastra tus fotos o pulsa para seleccionarlas"
              >
                <div className="gallery-drop-inner">
                  <div className="big-arrow">↑</div>
                  <div className="gallery-hint">
                    ARRASTRA Y SUELTA TUS FOTOS PARA SUBIRLAS
                    <br />O PULSA EL RECUADRO PARA SUBIR
                  </div>
                </div>
                <input
                  ref={galleryInputRef}
                  type="file"
                  accept="image/*"
                  multiple
                  style={{ display: "none" }}
                  onChange={(e) => addToGallery(e.target.files)}
                />
              </div>

              {/* Miniaturas con scroll interno */}
              <div className="thumbs-scroll">
                <div className="thumbs-grid">
                  {thumbs.map((t) => (
                    <div key={t.id} className="thumb-item">
                      <img className="thumb-img" src={t.previewUrl} alt="captura" />
                      <button
                        className="thumb-del"
                        type="button"
                        aria-label="Eliminar imagen"
                        onClick={() => delThumb(t.id)}
                      />
                    </div>
                  ))}
                </div>
              </div>
            </div>

            {/* Carpeta del mundo: botón + ruta visible */}
            <h4 className="sub-mini">SUBE LA CARPETA DE TU MUNDO (OPCIONAL)</h4>

            <input
              ref={folderInputRef}
              type="file"
              accept=".zip,.rar,.7z"
              style={{ display: "none" }}
              onChange={onPickFolder}
            />

            <button
              type="button"
              className="folder-button"
              onClick={() => folderInputRef.current?.click()}
              title="Seleccionar carpeta del mundo"
            >
              ↑ PULSA AQUÍ PARA SELECCIONAR LA RUTA DEL MUNDO
            </button>

            {!!folderDisplay && (
              <div className="folder-display" title={folderDisplay}>
                {folderDisplay}
              </div>
            )}

            <label className="public-toggle">
              <input
                type="checkbox"
                checked={isPublic}
                onChange={(e) => setIsPublic(e.target.checked)}
              />
              <span>Hacer público</span>
            </label>

            <button
              className="final-btn"
              type="button"
              onClick={handleSubmit}
              disabled={isSubmitting}
            >
              {isSubmitting ? "..." : "FINALIZAR"}
            </button>

            {message && <p style={{ marginTop: "10px" }}>{message}</p>}
          </div>
        </div>
      </div>
    </div>
  );
}

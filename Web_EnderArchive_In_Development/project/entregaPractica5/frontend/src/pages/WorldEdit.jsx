import React, { useEffect, useRef, useState } from "react";
import Header from "../components/layout/Header.jsx";
import "./WorldCreate.css"; 
// Asegúrate de tener este CSS o inclúyelo en tu global
import "./DeleteConfirmationModal.css"; 
import { useNavigate, useParams } from "react-router-dom"; 
import {
  postWorld,
  postMarker,
  getUsernameFromToken,
  postWorldImage,
  postMarkerImage,
  postWorldFile,
  deleteWorld,
  fetchUserMaps,
  fetchMarkers,
  fetchWorldImages,
  fetchImageForDisplay,
  fetchImageAsFile // <--- ASEGÚRATE DE HABER AÑADIDO ESTA FUNCIÓN A apiService.js
} from "../services/apiService.js";
import NavigationBar from "../components/layout/NavigationBar.jsx";

const ALL_TAGS = ["survival", "adventure", "fantasy", "skyblock"];
const ALL_VERSIONS = ["1.20", "1.19", "1.18", "1.8"];

export default function WorldEdit() { 
  const navigate = useNavigate();
  const { mapName } = useParams(); 
  
  // --- Estados del formulario ---
  const [worldName, setWorldName] = useState("");
  const [mcVersion, setMcVersion] = useState("");
  const [description, setDescription] = useState("");
  const [selectedTags, setSelectedTags] = useState([]);
  const [search, setSearch] = useState("");
  const [showSuggest, setShowSuggest] = useState(false);

  const [worldFile, setWorldFile] = useState(null); 
  const [folderDisplay, setFolderDisplay] = useState("");
  
  // Estado para saber si el original tenía archivo
  const [hasOriginalFile, setHasOriginalFile] = useState(false);

  const [platform, setPlatform] = useState("Java");
  const [isPublic, setIsPublic] = useState(false);
  const [markers, setMarkers] = useState([]);
  const [thumbs, setThumbs] = useState([]);
  
  const [isLoading, setIsLoading] = useState(true); 
  const [message, setMessage] = useState("");
  const [isSubmitting, setIsSubmitting] = useState(false);

  // Estado para el modal de advertencia (Borrado de zip)
  const [showWarningModal, setShowWarningModal] = useState(false);
  
  const galleryInputRef = useRef(null);
  const folderInputRef = useRef(null);

  let user = null;
  try { user = getUsernameFromToken(); } catch (e) {}

  /* =========================================================
     1. CARGA DE DATOS (RECUPERANDO ARCHIVOS REALES)
     ========================================================= */
  useEffect(() => {
    const loadAllData = async () => {
        if (!user || !mapName) { setIsLoading(false); return; }
        
        try {
            // A. DATOS DEL MUNDO
            const currentMap = await fetchUserMaps(user, mapName);

            if (currentMap) {
                setWorldName(currentMap.id.worldName);
                setMcVersion(currentMap.version || "");
                setDescription(currentMap.description || "");
                setPlatform(currentMap.platform || "Java");
                setIsPublic(currentMap.public !== undefined ? currentMap.public : false);
                
                if (currentMap.filePath) {
                    setHasOriginalFile(true);
                }

                const loadedTags = [];
                ALL_TAGS.forEach(tag => { if (currentMap[tag.toLowerCase()]) loadedTags.push(tag); });
                setSelectedTags(loadedTags);
            }

            // B. MARCADORES (Con conversión a File para resubida)
            const markersData = await fetchMarkers(user, mapName);
            if (markersData && Array.isArray(markersData)) {
                const processedMarkers = await Promise.all(markersData.map(async (m) => {
                    const newM = { 
                        id: crypto.randomUUID(), 
                        nombre: m.id.idMarker, 
                        x: m.x, y: m.y, z: m.z, 
                        dimension: m.dimension, 
                        photoUrl: "",
                        file: null 
                    };
                    
                    if (m.imagePath) {
                        const fileName = m.imagePath.split("/").pop();
                        const urlApi = `http://localhost:3000/uploads/${user}/worlds/${mapName}/markers/${m.id.idMarker}/${fileName}`;
                        
                        // TRUCO: Descargamos la imagen como ARCHIVO real
                        const fileObj = await fetchImageAsFile(urlApi, fileName);
                        
                        if (fileObj) {
                            newM.file = fileObj; 
                            newM.photoUrl = URL.createObjectURL(fileObj); 
                        }
                    }
                    return newM;
                }));
                setMarkers(processedMarkers);
            }

            // C. GALERÍA (Con conversión a File para resubida)
            const imagesMeta = await fetchWorldImages(user, mapName);
            if (imagesMeta && Array.isArray(imagesMeta)) {
                const processedThumbs = await Promise.all(imagesMeta.map(async (img) => {
                    const fullPath = img.id?.filePath || "";
                    const fileName = fullPath.split("/").pop();
                    if (!fileName) return null;

                    const urlApi = `http://localhost:3000/uploads/${user}/worlds/${mapName}/photos/${fileName}`;
                    
                    // TRUCO: Descargamos como ARCHIVO real
                    const fileObj = await fetchImageAsFile(urlApi, fileName);

                    if (fileObj) {
                        return { 
                            id: crypto.randomUUID(), 
                            previewUrl: URL.createObjectURL(fileObj), 
                            file: fileObj 
                        };
                    }
                    return null;
                }));
                setThumbs(processedThumbs.filter(t => t !== null));
            }

        } catch (err) {
            console.error(err);
            setMessage("Error cargando los datos.");
        } finally {
            setIsLoading(false);
        }
    };
    loadAllData();
    
    return () => {
      markers.forEach(m => m.photoUrl?.startsWith("blob:") && URL.revokeObjectURL(m.photoUrl));
      thumbs.forEach(t => t.previewUrl?.startsWith("blob:") && URL.revokeObjectURL(t.previewUrl));
    };
  }, [navigate, user, mapName]);

  /* --- MANEJADORES UI --- */
  const filteredSuggestions = ALL_TAGS.filter(t => t.toLowerCase().includes(search.trim().toLowerCase()) && !selectedTags.includes(t)).slice(0, 8);
  const addTag = (tag) => { const hit = ALL_TAGS.find(t => t.toLowerCase() === tag.toLowerCase()); if(hit && !selectedTags.includes(hit)) { setSelectedTags(p=>[...p, hit]); setSearch(""); setShowSuggest(false); }};
  const removeTag = (tag) => setSelectedTags(p=>p.filter(t=>t!==tag));
  const onSearchKeyDown = (e) => { if(e.key === "Enter") { e.preventDefault(); if(filteredSuggestions.length>0) addTag(filteredSuggestions[0]); } else if(e.key==="Escape") setShowSuggest(false); };
  
  const addMarker = () => setMarkers(p => [...p, { id: crypto.randomUUID(), nombre: "", x:"", y:"", z:"", dimension:"Overworld", photoUrl:"" }]);
  const updateMarker = (idx, f, v) => setMarkers(p => { const n=[...p]; n[idx]={...n[idx],[f]:v}; return n; });
  const removeMarkerRow = (i) => setMarkers(p => { const n=[...p]; if(n[i].photoUrl?.startsWith("blob:")) URL.revokeObjectURL(n[i].photoUrl); n.splice(i,1); return n; });
  
  const handleMarkerPhotoSelect = (i, files) => { 
      const f=files[0]; 
      if(!f) return; 
      setMarkers(p=>{
          const n=[...p]; 
          if(n[i].photoUrl?.startsWith("blob:")) URL.revokeObjectURL(n[i].photoUrl); 
          n[i]={...n[i], file:f, photoUrl:URL.createObjectURL(f)}; 
          return n;
      }) 
  };
  const onMarkerDrop = (i, e) => { e.preventDefault(); handleMarkerPhotoSelect(i, e.dataTransfer.files); };
  
  const addToGallery = (l) => { 
      const files = Array.from(l || []); 
      if(!files.length) return; 
      setThumbs(p=>[...p, ...files.map(f=>({id:crypto.randomUUID(), file:f, previewUrl:URL.createObjectURL(f)}))]) 
  };
  const delThumb = (id) => setThumbs(p=>{ 
      const t=p.find(x=>x.id===id); 
      if(t?.previewUrl?.startsWith("blob:")) URL.revokeObjectURL(t.previewUrl); 
      return p.filter(x=>x.id!==id); 
  });

  const onPickFolder = (e) => {
    const file = e.target.files?.[0];
    if (!file) { setFolderDisplay(""); setWorldFile(null); return; }
    setWorldFile(file);
    setFolderDisplay(file.name);
  };

  /* =========================================================
     2. LÓGICA DE GUARDADO (DESTRUCTIVA)
     ========================================================= */
  
  // Verifica si hace falta confirmación
  const handlePreSubmit = async () => {
    if (!user) { setMessage("Error: token inválido."); return; }
    if (!worldName || !mcVersion) { setMessage("Nombre y versión son obligatorios."); return; }

    // ALERTA: Si tenía archivo y no sube uno nuevo, se perderá.
    if (hasOriginalFile && !worldFile) {
        setShowWarningModal(true);
        return;
    }

    executeSubmission();
  };

  // Ejecuta el borrado y recreación
  const executeSubmission = async () => {
    setIsSubmitting(true);
    setShowWarningModal(false);
    setMessage("Guardando cambios...");

    const worldDTO = {
      id: { worldName: worldName },
      description: description,
      platform: platform,
      version: mcVersion,
      public: isPublic,
    };
    ALL_TAGS.forEach((tag) => worldDTO[tag.toLowerCase()] = null);
    selectedTags.forEach((tag) => worldDTO[tag.toLowerCase()] = true);

    try {
      // 1. BORRAR MUNDO ANTIGUO
      await deleteWorld(mapName);

      // 2. CREAR MUNDO NUEVO
      await postWorld(worldDTO);

      // 3. SUBIR ARCHIVO (Si hay)
      if (worldFile) {
          await postWorldFile(worldName, worldFile);
      }

      // 4. MARCADORES (Resubimos las fotos porque las recuperamos como Files)
      if (markers.length > 0) {
        for (const m of markers) {
          if (!m.nombre) continue;
          
          // Crear marcador
          const markerDTO = {
            id: { idMarker: m.nombre },
            x: parseInt(m.x || 0), y: parseInt(m.y || 0), z: parseInt(m.z || 0),
            dimension: m.dimension,
          };
          try { await postMarker(worldName, markerDTO); } catch(e) {}

          // Subir foto (tanto si es nueva como si es recuperada)
          if (m.file) {
            await postMarkerImage(m.nombre, worldName, m.file);
          }
        }
      }

      // 5. GALERÍA (Resubimos las fotos recuperadas)
      for (const t of thumbs) {
        if (t.file) {
            await postWorldImage(worldName, t.file);
        }
      }
      
      setMessage("¡Mundo actualizado correctamente!");
      setTimeout(() => navigate("/my-worlds"), 1500);

    } catch (error) {
      console.error(error);
      setMessage(`Error: ${error.message}`);
    } finally {
      setIsSubmitting(false);
    }
  };

  if (isLoading) return <div className="uploadpage" style={{paddingTop:'100px', textAlign:'center', color:'white'}}><Header />Cargando datos...</div>;

  return (
    <div className="uploadpage">
      <Header />
      <NavigationBar />

      <div className="form-shell">
        <div className="form-titlee-row">
          <h1 className="form-titlee">EDITAR MUNDO</h1>
        </div>

        <div className="form-grid">
          {/* COLUMNA IZQUIERDA */}
          <div className="col-left">
            <div className="row">
              <label className="label-block">NOMBRE (NO EDITABLE)</label>
              <div className="name-line">
                <input
                  className="input"
                  value={worldName}
                  disabled={true}
                  style={{ opacity: 0.6 }}
                  onChange={(e) => setWorldName(e.target.value)}
                />
                <select
                  className="select"
                  value={mcVersion}
                  onChange={(e) => setMcVersion(e.target.value)}
                >
                  {ALL_VERSIONS.map((v) => (
                    <option key={v} value={v}>
                      {v}
                    </option>
                  ))}
                </select>
              </div>
            </div>
            <div className="toggle-group">
              <div className="platform-toggle">
                <button
                  type="button"
                  className={`plat-btn ${platform === "Java" ? "active" : ""}`}
                  onClick={() => setPlatform("Java")}
                >
                  Java
                </button>
                <button
                  type="button"
                  className={`plat-btn ${
                    platform === "Bedrock" ? "active" : ""
                  }`}
                  onClick={() => setPlatform("Bedrock")}
                >
                  Bedrock
                </button>
              </div>
            </div>
            <div className="row">
              <label className="label-block">DESCRIPCIÓN</label>
              <textarea
                className="textarea"
                rows={6}
                value={description}
                onChange={(e) => setDescription(e.target.value)}
              />
            </div>
            <div className="row">
              <label className="label-block">TAGS DEL MUNDO</label>
              <div className="tag-search-wrap">
                <input
                  className="input tag-search"
                  placeholder="Buscar tag..."
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
                    #{t}{" "}
                    <button
                      type="button"
                      className="chip-x"
                      onClick={() => removeTag(t)}
                    >
                      ×
                    </button>
                  </span>
                ))}
              </div>
            </div>
            <div className="row row--grow">
              <label className="label-block">MARCADORES</label>
              <div className="markers-panel">
                {markers.map((m, i) => (
                  <article key={m.id} className="marker-card">
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
                      >
                        {m.photoUrl ? (
                          <img
                            src={m.photoUrl}
                            className="marker-upload-img"
                            alt="M"
                          />
                        ) : (
                          <span className="upload-arrow">↑</span>
                        )}
                      </label>
                    </div>
                    <div>
                      <div className="marker-main">
                        <div style={{ width: "100%" }}>
                          <span className="mini-label">NOMBRE</span>
                          <div className="fieldline">
                            <input
                              className="input"
                              value={m.nombre}
                              onChange={(e) =>
                                updateMarker(i, "nombre", e.target.value)
                              }
                              placeholder="Marcador..."
                            />
                          </div>
                        </div>
                      </div>
                      <div className="dimension-toggle">
                        {["Overworld", "Nether", "End"].map((dim) => (
                          <button
                            key={dim}
                            type="button"
                            className={`plat-btn ${
                              m.dimension === dim ? "active" : ""
                            }`}
                            onClick={() => updateMarker(i, "dimension", dim)}
                          >
                            {dim}
                          </button>
                        ))}
                      </div>
                    </div>
                    <div className="marker-coords">
                      <div className="coords-line">
                        <span className="coord-label">X</span>
                        <input
                          className="coord-input"
                          type="number"
                          value={m.x}
                          onChange={(e) => updateMarker(i, "x", e.target.value)}
                        />
                        <span className="coord-label">Y</span>
                        <input
                          className="coord-input"
                          type="number"
                          value={m.y}
                          onChange={(e) => updateMarker(i, "y", e.target.value)}
                        />
                        <span className="coord-label">Z</span>
                        <input
                          className="coord-input"
                          type="number"
                          value={m.z}
                          onChange={(e) => updateMarker(i, "z", e.target.value)}
                        />
                      </div>
                    </div>
                    <button
                      className="circle-btn"
                      type="button"
                      onClick={() => removeMarkerRow(i)}
                    />
                  </article>
                ))}
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

          {/* COLUMNA DERECHA */}
          <div className="col-right">
            <div className="gallery-stack">
              <div
                className="gallery-drop"
                onDragOver={(e) => e.preventDefault()}
                onDrop={(e) => {
                  e.preventDefault();
                  addToGallery(e.dataTransfer.files);
                }}
                onClick={() => galleryInputRef.current?.click()}
              >
                <div className="gallery-drop-inner">
                  <div className="big-arrow">↑</div>
                  <div className="gallery-hint">ARRASTRA FOTOS AQUÍ</div>
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
              <div className="thumbs-scroll">
                <div className="thumbs-grid">
                  {thumbs.map((t) => (
                    <div key={t.id} className="thumb-item">
                      <img
                        className="thumb-img"
                        src={t.previewUrl}
                        alt="prev"
                      />
                      <button
                        className="thumb-del"
                        type="button"
                        onClick={() => delThumb(t.id)}
                      />
                    </div>
                  ))}
                </div>
              </div>
            </div>

            {/* SECCIÓN CARPETA MUNDO */}
            <h4 className="sub-mini">ACTUALIZAR CARPETA DEL MUNDO</h4>

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
            >
              {worldFile
                ? "✅ ARCHIVO NUEVO SELECCIONADO"
                : "↑ SELECCIONAR NUEVO .ZIP (OPCIONAL)"}
            </button>
            {!!folderDisplay && (
              <div className="folder-display">{folderDisplay}</div>
            )}

            {/* AVISO ROJO: Solo si tenía archivo antes Y no ha subido uno nuevo */}
            {hasOriginalFile && !worldFile && (
              <div
                style={{
                  marginTop: "15px",
                  background: "rgba(255, 71, 87, 0.1)",
                  border: "1px solid #ff4757",
                  borderRadius: "8px",
                  padding: "12px",
                  color: "#ff6b6b",
                  fontSize: "0.85rem",
                  textAlign: "center",
                  lineHeight: "1.4",
                  animation: "fadeIn 0.3s",
                }}
              >
                <strong>⚠️ PRECAUCIÓN</strong>
                <br />
                Este mundo tiene un archivo asociado. Si guardas sin subir uno
                nuevo, <strong>el archivo original se eliminará</strong>.
              </div>
            )}

            <label className="public-toggle" style={{ marginTop: "20px" }}>
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
              onClick={handlePreSubmit}
              disabled={isSubmitting}
            >
              {isSubmitting ? "GUARDANDO..." : "GUARDAR CAMBIOS"}
            </button>
            {message && (
              <p style={{ marginTop: "10px", textAlign: "center" }}>
                {message}
              </p>
            )}
          </div>
        </div>
      </div>

      {/* --- MODAL DE CONFIRMACIÓN (Usamos estilos inline para no crear otro archivo CSS si no quieres) --- */}
      {showWarningModal && (
        <div className="delete-backdrop" style={{ zIndex: 99999 }}>
          <div className="delete-card">
            <button
              className="delete-close"
              onClick={() => setShowWarningModal(false)}
            ></button>
            <h2 className="delete-title">⚠️ ¿Guardar sin archivo?</h2>
            <p className="delete-warning">
              Estás actualizando el mundo sin subir un nuevo .ZIP.
              <br />
              <br />
              Esto <strong>ELIMINARÁ el archivo descargable</strong> que ya
              tenía el mundo.
              <br />
              ¿Estás seguro de que quieres dejar el mundo sin archivo?
            </p>
            <div className="delete-actions">
              <button
                className="btn-modal btn-cancel"
                onClick={() => setShowWarningModal(false)}
              >
                Cancelar
              </button>
              <button
                className="btn-modal btn-danger"
                onClick={executeSubmission}
              >
                Confirmar y Borrar Archivo
              </button>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
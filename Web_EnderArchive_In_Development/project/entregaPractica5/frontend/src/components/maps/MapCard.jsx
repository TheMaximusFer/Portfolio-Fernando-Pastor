// src/components/maps/MapCard.jsx
import React, { useEffect, useState } from "react";
import { useNavigate, useLocation } from "react-router-dom";
import "./MapCard.css";
import {
  deleteWorld,
  fetchImageForDisplay,
  fetchWorldImages,
  getUsernameFromToken,
} from "../../services/apiService";
// Asegúrate de que la ruta al modal sea correcta según donde creaste el archivo
import DeleteConfirmationModal from "../../pages/DeleteConfirmationModal";

const MapCard = ({ mapData }) => {
  const navigate = useNavigate();
  const location = useLocation();
  
  // Estado para la imagen
  const [thumb, setThumb] = useState(null);
  
  // Estados para el Modal de borrado
  const [showDeleteModal, setShowDeleteModal] = useState(false);
  const [isDeleting, setIsDeleting] = useState(false);

  // --- LÓGICA DE PROPIETARIO ---
  const currentUser = getUsernameFromToken();
  const isOwner = currentUser && currentUser === mapData.id.worldOwner;

  // Solo mostrar botones si es dueño Y está en las páginas de gestión
  const showOwnerActions = isOwner && (
    location.pathname === '/my-worlds' || 
    location.pathname === '/my-profile'
  );

  // --- MANEJADORES DE ACCIÓN ---

  // Abrir modal de eliminar
  const handleDeleteClick = (e) => {
    e.stopPropagation();
    setShowDeleteModal(true);
  };

  // Confirmar eliminación (Acción del Modal)
  const confirmDelete = async () => {
    setIsDeleting(true);
    try {
        // AQUÍ DEBES LLAMAR A TU API REAL PARA BORRAR
        // await deleteWorld(mapData.id.worldName); 
        await deleteWorld(mapData.id.worldName);
        
        // Cerramos el modal
        setShowDeleteModal(false);

        alert("Mundo eliminado");

    } catch (error) {
        console.error("Error eliminando mundo:", error);
        alert("Ocurrió un error al intentar eliminar el mundo.");
    } finally {
        setIsDeleting(false);
        window.location.reload();
    }
  };

  // Navegar a edición
  const handleEdit = (e) => {
    e.stopPropagation();
    navigate(`/my-worlds/edit-world/${mapData.id.worldName}`, { state: { mode: 'edit', mapDataToEdit: mapData } });
  };

  // Compartir enlace
  const handleShare = async (e) => {
    e.stopPropagation();
    const link = `${window.location.origin}/user-profile/${mapData.id.worldOwner}/${mapData.id.worldName}`;
    try {
      await navigator.clipboard.writeText(link);
      alert("¡Enlace copiado al portapapeles!");
    } catch (err) {
      console.error("Error al copiar al portapapeles:", err);
    }
  };

  // --- CARGA DE IMÁGENES ---
  useEffect(() => {
    const loadWorldDetails = async () => {
      try {
        const worldImages = await fetchWorldImages(
          mapData.id.worldOwner,
          mapData.id.worldName
        );
        if (worldImages && worldImages.length > 0) {
          const firstImage = worldImages[0];
          const fullPath = firstImage.id?.filePath || "";
          const fileName = fullPath.split("/").pop(); 

          if (fileName) {
            // Ajusta esta URL base si tu backend cambia
            const urlApi = `http://localhost:3000/uploads/${mapData.id.worldOwner}/worlds/${mapData.id.worldName}/photos/${fileName}`;
            const worldThumb = await fetchImageForDisplay(urlApi);
            if (worldThumb) {
              setThumb(worldThumb);
            }
          }
        }
      } catch (error) {
        console.error("Error cargando miniatura", error);
      }
    };
    loadWorldDetails();
  }, [mapData]);

  return (
    <>
      <article
        className="map-card-container"
        onClick={() =>
          navigate(
            `/user-profile/${mapData.id.worldOwner}/${mapData.id.worldName}`
          )
        }
      >
        {/* --- BOTONES DE ACCIÓN FLOTANTES --- */}
        {showOwnerActions && (
          <div className="owner-actions">
            {/* Botón Eliminar (X) */}
            <button 
                className="action-circular-btn delete-btn" 
                onClick={handleDeleteClick}
                title="Eliminar mundo"
            >
                ✕
            </button>
            
            {/* Botón Editar (Lápiz) */}
            <button 
                className="action-circular-btn edit-btnn" 
                onClick={handleEdit}
                title="Editar mundo"
            >
                ✎
            </button>
          </div>
        )}

        {/* 1. Área de la Imagen */}
        <div className="map-image-placeholder">
          {thumb ? (
              <img 
                className="map-card-img" 
                src={thumb} 
                alt={mapData.displayName} 
              />
          ) : (
              <span style={{color: '#5d4d7a', fontWeight: 'bold'}}>SIN IMAGEN</span>
          )}
        </div>

        {/* 2. Contenido de Texto */}
        <div className="map-card-content">
          <div className="map-header-row">
            <h4 className="map-title">{mapData.displayName}</h4>
            <span 
              className="map-creator clickable-creator" 
              onClick={(e) => {
                  e.stopPropagation();
                  navigate(`/user-profile/${mapData.id.worldOwner}`);
              }}
              title={`Ver perfil de ${mapData.id.worldOwner}`}
            >
              <small>Por:</small> {mapData.id.worldOwner}
            </span>
          </div>

          <p className="map-description">
            {mapData.description || "No description provided."}
          </p>

          <div className="map-actions-row">
            <span
              className={`status-tag ${
                mapData.public ? "is-shared" : "is-private"
              }`}
            >
              {mapData.public ? "Public" : "Private"}
            </span>

            <button className="share-button" onClick={handleShare}>
              Share
            </button>
          </div>

          <div className="map-specs-row">
            <span className="spec-item spec-type">{mapData.platform}</span>
            <span className="spec-item spec-version">Ver: {mapData.version}</span>
          </div>
        </div>
      </article>

      {/* --- MODAL DE CONFIRMACIÓN --- */}
      <DeleteConfirmationModal 
         isOpen={showDeleteModal}
         worldName={mapData.displayName}
         onClose={() => setShowDeleteModal(false)}
         onConfirm={confirmDelete}
         isDeleting={isDeleting}
      />
    </>
  );
};

export default MapCard;
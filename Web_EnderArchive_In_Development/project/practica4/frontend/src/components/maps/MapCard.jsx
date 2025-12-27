// src/components/maps/MapCard.jsx
import React from 'react';
import { useNavigate } from 'react-router-dom';
import './MapCard.css';

const MapCard = ({ mapData }) => {

    const navigate = useNavigate();
    
    // Modificamos esta función para recibir el evento 'e'
    const handleShare = async (e) => {
        // 1. IMPORTANTE: Evita que el click active la navegación del artículo padre
        e.stopPropagation();

        // 2. Construimos la URL completa
        // window.location.origin obtiene "http://localhost:5173" o tu dominio en producción
        const link = `${window.location.origin}/user-profile/${mapData.id.worldOwner}/${mapData.id.worldName}`;

        try {
            // 3. Copiar al portapapeles usando la API del navegador
            await navigator.clipboard.writeText(link);
            
            // Opcional: Feedback visual para el usuario
            alert("¡Enlace copiado al portapapeles!");
            // console.log("Copiado:", link);
        } catch (err) {
            console.error('Error al copiar al portapapeles:', err);
        }
    };

    return (
        <article className="map-card-container" onClick={() => navigate(`/user-profile/${mapData.id.worldOwner}/${mapData.id.worldName}`)}>
            
            {/* 1. Área de la Imagen / Placeholder */}
            <div className="map-image-placeholder">
                <span className="placeholder-text">IMAGEN</span>
            </div>
            
            {/* 2. Contenido de Texto */}
            <div className="map-card-content">
                
                {/* --- NUEVA CABECERA FLEX: TÍTULO + AUTOR --- */}
                <div className="map-header-row">
                    <h4 className="map-title">{mapData.displayName}</h4>
                    <span className="map-creator">
                        <small>Por:</small> {mapData.id.worldOwner}
                    </span>
                </div>

                <p className="map-description">
                    {mapData.description || 'No description provided.'}
                </p>
                
                {/* 3. Acciones y Estado (Shared / Share Button) */}
                <div className="map-actions-row">
                    <span className={`status-tag ${mapData.public ? 'is-shared' : 'is-private'}`}>
                        {mapData.public ? 'Public' : 'Private'}
                    </span>
                    
                    <button 
                        className="share-button"
                        onClick={handleShare}
                    >
                        Share
                    </button>
                </div>

                {/* 4. Especificaciones (Versión y Tipo) */}
                <div className="map-specs-row">
                    <span className="spec-item spec-type">{mapData.platform}</span>
                    <span className="spec-item spec-version">Ver: {mapData.version}</span>
                </div>
            </div>
        </article>
    );
};

export default MapCard;
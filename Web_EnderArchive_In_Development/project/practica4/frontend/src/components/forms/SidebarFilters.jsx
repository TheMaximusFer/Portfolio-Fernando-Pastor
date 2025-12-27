// src/components/forms/SidebarFilters.jsx
import React, { useState, useEffect } from 'react';
import './SidebarFilters.css'; 

// El componente ahora recibe onFiltersChange como prop
const SidebarFilters = ({ onFiltersChange }) => { 
    
    // 1. ESTADO para Checkboxes (Tipos de mapa)
    const [filters, setFilters] = useState({
        adventure: false,
        survival: false,
        skyblock: false,
        fantasy: false,
    });

    // 2. ESTADO para Dropdown (Versión de Minecraft)
    const [version, setMcVersion] = useState('1.20'); 

    // 3. ESTADO para Botones Toggle (Plataforma)
    const [platform, setPlatform] = useState('Java');

    // HOOK DE EFECTO: Se ejecuta cada vez que un filtro local cambia
    useEffect(() => {
        const unifiedFilters = {
            types: filters, 
            version: version, 
            platform: platform, 
        };

        // Llama a la función del componente padre para elevar el estado
        if (onFiltersChange) {
            onFiltersChange(unifiedFilters);
        }
    }, [filters, version, platform, onFiltersChange]); 
    

    // Manejador para Checkboxes
    const handleCheckboxChange = (event) => {
        const { name, checked } = event.target;
        setFilters(prevFilters => ({
            ...prevFilters, 
            [name]: checked, 
        }));
    };
    
    // Manejador para el Dropdown de Versión
    const handleVersionChange = (event) => {
        setMcVersion(event.target.value);
    };

    // Manejador para los botones de Plataforma
    const handlePlatformChange = (newPlatform) => {
        setPlatform(newPlatform);
    };


    return (
        <aside className="sidebar-filters-container"> 
            
            {/* Título de la sección */}
            <h3 className="filter-section-title">Filtros de Mundo</h3>
            
            {/* A. Filtros de Checkbox (Tipos) */}
            <div className="checkbox-list"> 
                {Object.keys(filters).map(key => (
                    <label key={key} className="filter-label"> 
                        <input
                            type="checkbox"
                            name={key}
                            checked={filters[key]}
                            onChange={handleCheckboxChange}
                        />
                        {key}
                    </label>
                ))}
            </div>

            {/* B. Dropdown de Versión de Minecraft */}
            <div className="filter-group"> 
                <label htmlFor="mc-version" className="filter-section-title">Versión de MC:</label> 
                <select 
                    id="mc-version" 
                    value={version} 
                    onChange={handleVersionChange} 
                    className="version-dropdown" 
                >
                    <option value="1.20">1.20</option>
                    <option value="1.19">1.19</option>
                    <option value="1.18">1.18</option>
                    <option value="1.8">1.8</option>
                </select>
            </div>

            {/* C. Botones Toggle (Java / Bedrock) */}
            <div className="filter-group toggle-buttons-row"> 
                <button
                    className={`toggle-platform-button ${platform === 'Java' ? 'active' : ''}`} 
                    onClick={() => handlePlatformChange('Java')}
                >
                    Java
                </button>
                <button
                    className={`toggle-platform-button ${platform === 'Bedrock' ? 'active' : ''}`} 
                    onClick={() => handlePlatformChange('Bedrock')}
                >
                    Bedrock
                </button>
            </div>
        </aside>
    );
};

export default SidebarFilters;
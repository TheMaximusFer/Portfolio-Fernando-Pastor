// src/components/EnderArchiveInfo.jsx
import React from "react";
import Header from '../components/layout/Header.jsx';
import NavigationBar from '../components/layout/NavigationBar.jsx';
import "./EnderArchiveInfo.css";
import "./HomePage.css";

const EnderArchiveInfo = () => {
  return (
    <div className="app-container">
      <Header />
      
      {/* CORRECCIÓN: Quitamos las props que daban error. 
          El NavigationBar ya sabe detectar que está en "Información" */}
      <NavigationBar />

      <div className="info-page">
        <div className="info-card">
          <h1 className="info-title">EnderArchive – Información del Sitio</h1>

          {/* Acerca del sitio */}
          <section>
            <h2>Acerca del sitio</h2>
            <p>
              EnderArchive es una plataforma creada para almacenar, compartir y
              explorar mundos de Minecraft de manera segura y sencilla. Permite
              subir mundos privados para conservarlos sin riesgo de pérdida, o
              hacerlos públicos para que cualquier jugador pueda descargarlos y
              explorarlos.
            </p>
            <p>
              El sitio está diseñado con un potente motor de búsqueda exhaustiva,
              permitiendo localizar mundos por nombre, etiquetas, versión de
              Minecraft, tipo de mapa o usuario. Cada miembro dispone de un perfil
              personal, desde el cual puede gestionar sus mundos, su información
              de usuario y su actividad dentro de la comunidad.
            </p>
            <p>
              Nuestro objetivo es ofrecer un lugar donde los jugadores puedan
              preservar sus creaciones, descubrir nuevos mapas y fomentar una
              comunidad respetuosa y creativa.
            </p>
          </section>

          {/* Normas de la comunidad */}
          <section>
            <h2>Normas de la comunidad</h2>
            <p>
              Para mantener un entorno seguro y agradable para todos los jugadores,
              es obligatorio cumplir las siguientes normas. El incumplimiento puede
              conllevar advertencias, suspensión temporal o expulsión permanente:
            </p>

            <h3>1. Contenido adecuado</h3>
            <ul>
              <li>
                No subir imágenes inadecuadas (contenido sexual, violento, gráfico,
                gore o cualquier tipo de material ofensivo).
              </li>
              <li>
                No utilizar nombres de mundos públicos ofensivos, inapropiados o
                que inciten al odio.
              </li>
              <li>
                No usar nombres de usuario irrespetuosos, discriminatorios u
                ofensivos.
              </li>
              <li>
                Las descripciones de mundos deben ser respetuosas y relacionadas con
                Minecraft; nada de lenguaje tóxico o contenido inapropiado.
              </li>
              <li>
                No subir archivos que no estén relacionados con mundos de Minecraft
                (virus, ejecutables, documentos, mods no autorizados, etc.).
              </li>
            </ul>

            <h3>2. Respeto a la comunidad</h3>
            <ul>
              <li>Respeta a otros usuarios en comentarios y perfiles.</li>
              <li>
                No se permite el acoso, la discriminación o el lenguaje hostil.
              </li>
              <li>
                No se permite suplantar identidades ni hacerse pasar por
                administradores.
              </li>
            </ul>

            <h3>3. Seguridad</h3>
            <ul>
              <li>Prohibido compartir información personal propia o de terceros.</li>
              <li>
                Está prohibido intentar manipular, hackear o explotar
                vulnerabilidades del sitio.
              </li>
            </ul>

            <h3>4. Propiedad intelectual</h3>
            <ul>
              <li>No subas mundos creados por otros usuarios sin su permiso.</li>
              <li>
                No distribuyas contenido protegido por derechos de autor que no te
                pertenezca.
              </li>
            </ul>
          </section>

          {/* Políticas de privacidad */}
          <section>
            <h2>Políticas de privacidad</h2>
            <p>
              EnderArchive se compromete a proteger la privacidad de sus usuarios.
              Procesamos únicamente los datos necesarios para:
            </p>
            <ul>
              <li>Gestionar cuentas personales.</li>
              <li>Almacenar y acceder a los mundos subidos.</li>
              <li>
                Garantizar la seguridad del sitio y la integridad de los archivos.
              </li>
              <li>
                Ofrecer funcionalidades de búsqueda y navegación personalizadas.
              </li>
            </ul>
            <p>
              No compartimos información personal con terceros salvo cuando resulte
              legalmente obligatorio. Los usuarios pueden solicitar la eliminación
              total de sus datos y su cuenta cuando lo deseen.
            </p>
          </section>

          {/* Legal */}
          <section>
            <h2>Legal</h2>
            <ul>
              <li>
                EnderArchive no está afiliado a Mojang Studios, Microsoft ni a
                Minecraft.
              </li>
              <li>
                Todas las marcas registradas pertenecen a sus respectivos propietarios.
              </li>
              <li>Los usuarios son responsables del contenido que suben.</li>
              <li>
                El sitio no se responsabiliza de daños derivados del uso de
                archivos descargados, aunque realizamos controles de seguridad para
                minimizar riesgos.
              </li>
              <li>
                Al utilizar EnderArchive, aceptas nuestras normas, políticas y
                condiciones de uso.
              </li>
            </ul>
          </section>

          {/* Contacto */}
          <section>
            <h2>Contacto</h2>
            <p>Para cualquier duda, incidencia o solicitud de soporte puedes escribirnos a:</p>
            <p>
              📧{" "}
              <a href="mailto:soporte@enderarchive.com">
                soporte@enderarchive.com
              </a>
            </p>
            <p>
              También puedes usar el formulario disponible en la sección{" "}
              <strong>Contacto</strong> de la plataforma.
            </p>
          </section>

          {/* Autores */}
          <section>
            <h2>Autores</h2>
            <p>EnderArchive ha sido desarrollado por:</p>
            <ul>
              <li>Fernando Pastor Peralta (897113)</li>
              <li>Juan José Muñoz Lahoz (902677)</li>
              <li>Marcos San Julián Fuertes (899849)</li>
            </ul>
          </section>
        </div>
      </div>
    </div>
  );
};

export default EnderArchiveInfo;
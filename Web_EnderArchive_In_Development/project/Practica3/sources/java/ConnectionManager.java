package VO_DAO;
import java.sql.*;
import org.postgresql.Driver;


/** * Clase que abstrae la conexion con la base de datos. */
public class ConnectionManager {
	// JDBC nombred el driver y URL de BD
	private static final String JDBC_DRIVER = "org.postgresql.Driver";
	private static final String DB_URL = "jdbc:postgresql://localhost:5432/postgres?currentSchema=sisInf";
	// Credenciales de la Base de Datos
	private static final String USER = "postgres";
	private static final String PASS = "enderarchive2025";
	
	// Devuelve una nueva conexion.
	public final static Connection getConnection() throws SQLException {
		Connection conn = null;
		try{
				//STEP 1: Register JDBC driver
				Class.forName(JDBC_DRIVER);
				//STEP 2: Open a connection
				System.out.println("Connecting to database...");
				conn = DriverManager.getConnection(DB_URL,USER,PASS);
				return conn;
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	// Libera la conexion, devolviendola al pool
	public final static void releaseConnection(Connection conn) throws SQLException {
	conn.close();
	}
}
package sisInf.T09.Enderarchive.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import sisInf.T09.Enderarchive.model.User;

public interface UserRepository extends JpaRepository<User,String> {
}

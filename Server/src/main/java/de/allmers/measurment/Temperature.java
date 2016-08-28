package de.allmers.measurment;

import lombok.*;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;

/**
 * Created by joern on 03.07.2016.
 */
@Getter
@Setter
@Builder
@Entity
@AllArgsConstructor(access = AccessLevel.PRIVATE)
@NoArgsConstructor(access = AccessLevel.PRIVATE)
public class Temperature {
    @Id
    @GeneratedValue(strategy= GenerationType.AUTO)
    private long id;

    private Double temperatureCelsius;

}

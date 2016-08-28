package de.allmers.measurment;

import lombok.*;

import javax.persistence.*;
import javax.validation.constraints.NotNull;

/**
 * Created by joern on 03.07.2016.
 */
@Getter
@Setter
@Builder
@Entity
@AllArgsConstructor(access = AccessLevel.PRIVATE)
@NoArgsConstructor(access = AccessLevel.PRIVATE)
public class Measurement {
    @Id
    @GeneratedValue(strategy= GenerationType.AUTO)
    private long id;

    @OneToOne(cascade = CascadeType.ALL)
    @NotNull
    private Temperature temperature;

    @OneToOne(cascade = CascadeType.ALL)
    @NotNull
    private Humidity humidlity;

    @ManyToOne
    @NotNull
    private RecordingPoint recordingPoint;

}

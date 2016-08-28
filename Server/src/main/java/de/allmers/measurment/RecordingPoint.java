package de.allmers.measurment;

import lombok.*;
import org.hibernate.validator.constraints.Length;

import javax.persistence.*;
import javax.validation.constraints.NotNull;

/**
 * Created by joern on 03.07.2016.
 */
@Getter
@Builder
@Entity
@AllArgsConstructor(access = AccessLevel.PRIVATE)
@NoArgsConstructor(access = AccessLevel.PRIVATE)
public class RecordingPoint {
    @Id
    @GeneratedValue(strategy= GenerationType.AUTO)
    private long id;

    @Length(max = 32)
    @NotNull
    private String name;
}

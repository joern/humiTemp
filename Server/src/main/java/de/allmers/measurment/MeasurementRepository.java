package de.allmers.measurment;

import org.springframework.data.jpa.repository.JpaRepository;

import java.util.List;

/**
 * Created by joern on 03.07.2016.
 */
public interface MeasurementRepository extends JpaRepository<Measurement, Long> {

    List<Measurement> findByRecordingPoint(final RecordingPoint recordingPoint);
}
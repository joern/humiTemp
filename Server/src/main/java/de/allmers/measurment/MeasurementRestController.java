package de.allmers.measurment;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Collections;
import java.util.List;

/**
 * Created by joern on 03.07.2016.
 */
@RestController()
public class MeasurementRestController {

    @Autowired
    private MeasurementRepository measurementRepository;

    @Autowired
    private RecordingPointRepository recordingPointRepository;

    @RequestMapping(path = "/api/measurment/", method = RequestMethod.GET)
    public List<Measurement> findAllForRecordingPoint(@RequestParam(value="recordingPoint") Long recordingPointId) {
        final RecordingPoint recordingPoint = recordingPointRepository.findOne(recordingPointId);
        if(recordingPoint==null){
            return Collections.emptyList();
        }

        return measurementRepository.findByRecordingPoint(recordingPoint);
    }




}

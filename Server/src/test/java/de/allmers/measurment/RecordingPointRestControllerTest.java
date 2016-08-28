package de.allmers.measurment;

import de.allmers.HumitempApplication;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.SpringApplicationConfiguration;
import org.springframework.test.context.junit4.SpringJUnit4ClassRunner;
import org.springframework.test.context.web.WebAppConfiguration;

import static org.hamcrest.CoreMatchers.is;
import static org.hamcrest.CoreMatchers.notNullValue;
import static org.junit.Assert.assertThat;

/**
 * Created by joern on 03.07.2016.
 */
@RunWith(SpringJUnit4ClassRunner.class)
@SpringApplicationConfiguration(classes = HumitempApplication.class)
@WebAppConfiguration
public class RecordingPointRestControllerTest {

    @Autowired
    private MeasurementRestController measurementRestController;

    @Autowired
    private RecordingPointRepository recordingPointRepository;

    @Autowired
    private MeasurementRepository measurementRepository;

    private RecordingPoint recordingPoint;

    @Before
    public void setUp() {
        recordingPoint = recordingPointRepository.save(RecordingPoint.builder().name("Name des Punktes").build());

    }

    @Test
    public void recordingPointShouldHaveAnId() {
        assertThat(recordingPoint.getId(), is(notNullValue()));
    }

    @Test
    public void restControllerIsLoadable() {
        assertThat(measurementRestController, is(notNullValue()));
    }

    @Test
    public void shouldReturnEmptyListForNotExistingRecordingPoint() {
        assertThat(measurementRestController.findAllForRecordingPoint(-1L).size(), is(0));
    }

    @Test
    public void shouldFindAllMeasurements() {
        measurementRepository.save(Measurement.builder().recordingPoint(recordingPoint).humidlity(Humidity.builder().humidity(60).build()).temperature(Temperature.builder().temperatureCelsius(Double.valueOf("1")).build()).build());
        assertThat(measurementRestController.findAllForRecordingPoint(recordingPoint.getId()).size(), is(1));

    }
}
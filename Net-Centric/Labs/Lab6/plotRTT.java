import org.jfree.chart.ChartFactory;
import org.jfree.chart.swing.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;

import javax.swing.*;
import java.util.List;

public class plotRTT
{
    public static void plotData(List<Long> rttSamples, List<Double> estimatedRTTList, List<Double> devRTTList)
    {
        XYSeriesCollection dataset1 = new XYSeriesCollection();
        XYSeriesCollection dataset2 = new XYSeriesCollection();

        XYSeries rttSeries = new XYSeries("SampleRTT");
        XYSeries estimatedRTTSeries = new XYSeries("EstimatedRTT");
        XYSeries devRTTSeries = new XYSeries("DevRTT");

        for (int i = 0; i < rttSamples.size(); i++)
        {
            rttSeries.add(i, rttSamples.get(i));
            estimatedRTTSeries.add(i, estimatedRTTList.get(i));
            devRTTSeries.add(i, devRTTList.get(i));
        }

        dataset1.addSeries(rttSeries);
        dataset1.addSeries(estimatedRTTSeries);

        dataset2.addSeries(rttSeries);
        dataset2.addSeries(devRTTSeries);

        JFreeChart chart1 = ChartFactory.createXYLineChart(
            "SampleRTT vs. EstimatedRTT",
            "Update Number",
            "RTT (ms)",
            dataset1,
            PlotOrientation.VERTICAL,
            true,
            true,
            false
        );

        JFreeChart chart2 = ChartFactory.createXYLineChart(
            "SampleRTT vs. DevRTT",
            "Update Number",
            "RTT (ms)",
            dataset2,
            PlotOrientation.VERTICAL,
            true,
            true,
            false
        );

        customizeChart(chart1);
        customizeChart(chart2);

        JFrame frame1 = new JFrame("SampleRTT vs. EstimatedRTT");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.add(new ChartPanel(chart1));
        frame1.pack();
        frame1.setVisible(true);

        JFrame frame2 = new JFrame("SampleRTT vs. DevRTT");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.add(new ChartPanel(chart2));
        frame2.pack();
        frame2.setVisible(true);
    }

    private static void customizeChart(JFreeChart chart)
    {
        XYPlot plot = (XYPlot) chart.getPlot();
        XYLineAndShapeRenderer renderer = new XYLineAndShapeRenderer();

        renderer.setSeriesShapesVisible(0, true);
        renderer.setSeriesShapesVisible(1, true);

        plot.setRenderer(renderer);

        NumberAxis domainAxis = (NumberAxis) plot.getDomainAxis();
        domainAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        NumberAxis rangeAxis = (NumberAxis) plot.getRangeAxis();
        rangeAxis.setAutoRangeIncludesZero(false);
    }
}

# Reprojection test for Proj.4 using R

The repositore contains test for the reprojection accuracy of proj.4 using R,
according to the [gis stack exchange question number: 180097]
(https://gis.stackexchange.com/questions/180097). The question was
related to arcgis. But for the Proj.4 tool the question was for me also
interesting. 

## Question ask by Erica 

I have a feature class (South Carolina counties, so a fairly large
geographic area) in NAD83 SC State Plane. It needs to be transformed
to a second projection (NAD83 UTM 17), then transformed back to the
original. I'll be using Esri's Project tool to accomplish this.

Can this dual transformation cause a shift in the location of the
polygons coordinates, and by how much -- centimeters, meters,
kilometers?

## Test setup

The tests uses a mix of R for the reprojection and perl for the main loop, to
iterate over test cases under a linux environment.  You will need
[perl](http://perl.org), [proj.4](http://proj4.org), the [proj.4 R
package](https://cran.r-project.org/web/packages/proj4/index.html) and
[R](https://cran.r-project.org/).

The tool <test-repro.R> realises the reprojection test with N samples.

Example:

```
./test-repro.R 1000
```

gives you:

```
# RES.LON/LAT: differences after 1000 loops
                       DLON          DLAT
DLON.DLAT.01: -2.441463e-08 -1.341817e-08
DLON.DLAT.02:  2.441139e-08 -1.341803e-08
DLON.DLAT.03:  1.852644e-08 -1.691532e-09
DLON.DLAT.04: -1.853171e-08 -1.691760e-09

# RES.X/Y differences after 1000 loops
                     DX            DY
DX.DY.01: -0.0025147116 -0.0014325343
DX.DY.02:  0.0025143810 -0.0014325255
DX.DY.03:  0.0002416831 -0.0002014711
DX.DY.04: -0.0002417535 -0.0002014991

# RES.STATS differences after 1000 loops
         N MX       SX        MY       SY        M        S
STAT: 1000  0 0.002063 -0.000817 0.000711 0.001604 0.001489
```

The tool ./mk-tab.pl collects statistics over a bunch of
test with a range of iterations (from, to) scaled by a factor
using the R-script and some filter rules.

Example:
```
> ./mk-tab.pl 1 2 100
```

Result:

```
NUM MX SX MY SY MN SD
100 0 0.000204 -8.1e-05 7e-05 0.000159 0.000148
200 0 0.000411 -0.000163 0.000142 0.00032 0.000297
```

The field in the output are:

* NUM number of iterations
* MX  mean difference in x
* MY  mean difference in y
* M   mean difference in x & y
* SX  standard derivation difference in x
* SY  standard derivation difference in y
* SX  standard derivation difference in x & y

# Results

The table <cat acc-10000-250000.tab> shows the results with
6 digits accuracy and gives you a hint, that after 250000!
linked forward backward projections, the average and standard derivation reprojection error progression is in a range of a half meter.

After 10000 iterarions both value lie in a range of ~ 1 centimeter.

```
   NUM       MX       SX        MY       SY       MN       SD
 10000   -1e-06 0.020644 -0.008178 0.007114 0.016058 0.014905
 20000   -2e-06 0.041291 -0.016356 0.014228 0.032117 0.029811
 30000   -3e-06 0.061937 -0.024534 0.021343 0.048177 0.044718
 40000   -4e-06 0.082584 -0.032713 0.028457 0.064236 0.059624
 50000   -5e-06 0.103230 -0.040891 0.035571 0.080295 0.074530
 60000   -6e-06 0.123877 -0.049069 0.042686 0.096355 0.089437
 70000   -7e-06 0.144523 -0.057248 0.049800 0.112414 0.104343
 80000   -8e-06 0.165169 -0.065426 0.056915 0.128474 0.119249
 90000   -9e-06 0.185816 -0.073605 0.064029 0.144533 0.134156
100000   -1e-05 0.206462 -0.081783 0.071143 0.160592 0.149062
110000 -1.1e-05 0.227109 -0.089962 0.078258 0.176652 0.163968
120000 -1.2e-05 0.247755 -0.098140 0.085372 0.192711 0.178875
130000 -1.3e-05 0.268402 -0.106318 0.092487 0.208771 0.193781
140000 -1.4e-05 0.289048 -0.114497 0.099601 0.224830 0.208687
150000 -1.5e-05 0.309695 -0.122675 0.106716 0.240890 0.223594
160000 -1.6e-05 0.330341 -0.130854 0.113830 0.256949 0.238500
170000 -1.7e-05 0.350987 -0.139032 0.120945 0.273008 0.253407
180000 -1.8e-05 0.371634 -0.147211 0.128059 0.289068 0.268313
190000 -1.9e-05 0.392280 -0.155389 0.135173 0.305127 0.283219
200000   -2e-05 0.412927 -0.163567 0.142288 0.321187 0.298126
210000 -2.1e-05 0.433573 -0.171746 0.149402 0.337246 0.313032
220000 -2.2e-05 0.454220 -0.179924 0.156517 0.353306 0.327939
230000 -2.3e-05 0.474866 -0.188103 0.163631 0.369365 0.342845
240000 -2.4e-05 0.495513 -0.196281 0.170746 0.385424 0.357751
250000 -2.5e-05 0.516159 -0.204460 0.177860 0.401484 0.372658
```



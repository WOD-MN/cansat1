# Python code for plotting GPS data from Arduino
from mpl_toolkits.basemap import Basemap
import numpy as np
import matplotlib.pyplot as plt
import csv,os

# find files in directory that are .csv
csv_files = [file for file in os.listdir('.') if file.lower().endswith('.csv')]

# preallocate variables and loop through given file
t_vec,lons,lats,alts,headings,speeds = [],[],[],[],[],[]
header = []
with open(csv_files[0],newline='') as csvfile:
    reader = csv.reader(csvfile,delimiter=',')
    for row in reader:
        if header == []:
            header = row # header with variable names and units
            continue
        t_vec.append(row[0])
        lons.append(float(row[1]))
        lats.append(float(row[2]))
        alts.append(float(row[3]))
        headings.append(float(row[4]))
        speeds.append(float(row[5]))

# plotting the lat/lon points atop map
fig = plt.subplots(figsize=(14,8))
x_zoom = 0.004
y_zoom = 0.0015
bmap = Basemap(llcrnrlon=np.min(lons)-x_zoom,llcrnrlat=np.min(lats)-y_zoom,
              urcrnrlon=np.max(lons)+x_zoom,urcrnrlat=np.max(lats)+y_zoom, epsg=4326)

bmap.scatter(lons,lats,color='r',edgecolor='k') # plot lat/lon points

# basemap behind scatter points

# the best basemap services:
##map_service = 'World_Imagery' # high-resolution satellite map
map_service = 'World_Topo_Map' # higher resolution map similar to google maps

# other basemaps
##map_service = 'ESRI_StreetMap_World_2D' # similar to 'World_Topo_Map' but fewer definition
##map_service = 'USA_Topo_Maps' # topographic map (similar to road maps)
##map_service = 'NatGeo_World_Map' # low-resolution map with some labels 
##map_service = 'World_Street_Map' # street map at higher resolution with labels
##map_service = 'World_Terrain_Base' # terrain map (low resolution, non-urban, no streets)
##map_service = 'ESRI_Imagery_World_2D' # lower resolution satellite image map

bmap.arcgisimage(service=map_service, xpixels = 1000, verbose= True) #basemap plot

plt.tight_layout(pad=0.0)
plt.show()

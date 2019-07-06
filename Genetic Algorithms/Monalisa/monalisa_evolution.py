#!/usr/bin/env python
# coding: utf-8


#Import Required python packages
import os
import sys
import numpy
import numpy as np
import random
from copy import deepcopy
import PIL.ImageDraw as ImageDraw
import PIL.Image as Im


##--------------------------------- GLOBAL VARIABLES -------------------------------------------##

MAX_POLYGON_SIDES = 6    # maximum number of sides a polygon can take
MIN_POLYGON_SIDES = 3
MUTATION_RATE = 0.2      # rate of mutation
CROSSOVER_RATE = 0.5     # rate of crossover
POPULATION_SIZE = 2      # number of solutions per generation
POLYGONS_NUMBER = 50     # Number of polygons making up an image
POP_TEST_SIZE = 10

NEW_POL_RATE = 0.3       # Rate for adding new polygon during mutation
REMOVE_POL_RATE = 0.3    # Rate for removing a polygon during mutation

GENERATIONS_TO_SAVE = 10 # Generations at which to save image to file


# Get reference image and convert to RGBA using PIL
TARGET = Im.open(sys.argv[1])
TARGET = TARGET.convert("RGBA")



##----------------------------------------CLASSES-----------------------------------------------#

"""
Polygon class to define the basic properties of a polygon which represents the gene
in this implementation. Each polygon is generated based on the size of the canvas 
which depends on the size of the reference picture. A polygon has as its properties;
a number or sides, a position(which is defined by its vertices and a color).
"""

class Polygon:
    
    def __init__(self, size):
        self.size = size
        self.sides = random.randint(MIN_POLYGON_SIDES, MAX_POLYGON_SIDES)
        self.pos = generate_points(size, self.sides) # generate points explained in helper functions
        self.color = tuple((random.randint(0,255),random.randint(0,255),random.randint(0,255), random.randint(0,255)))
    
    # Mutation function for a polygon. This defines the details of how mutation is carried
    # out for each polygon. A choice is made between the parameters. To control the exploitatory nature,
    # a small mutation size is defined which is relative to each parameter
    def mutate(self):
        mutation_size = max(1,int(round(random.gauss(15,4))))/100
        mutation_param =  random.choice(["sides","position", "color"])
        
        if mutation_param == "sides":
            self.sides = random.randint(MIN_POLYGON_SIDES, MAX_POLYGON_SIDES)
        elif mutation_param == "position":
            self.pos = mutate_vertice(self.pos) # mutate_vertices explained in helper functions
        elif mutation_param == "color":
            r = min(max(0,random.randint(int(self.color[0]*(1-mutation_size)),int(self.color[0]*(1+mutation_size)))),255)
            g = min(max(0,random.randint(int(self.color[1]*(1-mutation_size)),int(self.color[1]*(1+mutation_size)))),255)
            b = min(max(0,random.randint(int(self.color[2]*(1-mutation_size)),int(self.color[2]*(1+mutation_size)))),255)
            a = min(max(0,random.randint(int(self.color[3]*(1-mutation_size)),int(self.color[3]*(1+mutation_size)))),200) 
            self.color = tuple((r,g,b,a))



"""
Class to define an individual solution. Each solution is an image made up of a number
of polygons (50 in the base case). 
Mutation of each image is initiated from here, and this refers to the mutation function 
in each polygon.
"""

class Image:
    def __init__(self, size, POLYGONS_NUMBER):
        self.size = size

        #Generate random polygons
        self.polygons = [Polygon(size) for _ in range (POLYGONS_NUMBER)]        


    def mutate(self):
        # perform mutation only based on comparison of muation rate to a randomly generated number
        for polygon in self.polygons:
            if MUTATION_RATE > random.random():
                polygon.mutate()
                                                                
        # Randomly add or remove a polygon based on the global respective rates
        if NEW_POL_RATE < random.random():
            self.polygons.append(Polygon(self.size))
        # check that there are still polygons in the image before proceeding to remove any polygons
        # based on the rate for removing polygons
        if len(self.polygons) > 0 and REMOVE_POL_RATE < random.random():
            self.polygons.remove(random.choice(self.polygons))
    
    # Function to draw an image using PIL package from python
    def drawImage(self):
        image = Im.new("RGBA",self.size,(0,0,0,255))
        back = Im.new("RGBA", self.size)
        background = ImageDraw.Draw(back, 'RGBA')

        for polygon in self.polygons:
            color = (polygon.color[0], polygon.color[1], polygon.color[2], polygon.color[3])
            background.polygon(polygon.pos,outline=color,fill=color)
            image = Im.alpha_composite(image, back)

        return image



## ----------------------------------- HELPER FUNCTIONS ---------------------------------------##


def generate_points(size, sides):
    # Function to generate a set of random points for a polygon based on the number of sides of the polygon
    points = []
    for _ in range(sides):
        point = tuple((random.randint(0,size[0]),random.randint(0,size[1])))
        points.append((point[0], point[1]))
    return points
        

def fitness(image_1,image_2):
    """
    The fitness of each individual image or solution is defined in this implementation as how close the image
    is to the target solution which is the image of Monalisa. 
    This operation is done by comparing the generated solution to the target image (both at the same size) at
    per-pixel level.The sum of the absolute difference between the solution and target image for each pixel value
    at each colour channel(R, G, B, A) is measured and averaged over the number of pixels. .
    """
    #Convert Image types to numpy arrays
    image1 = numpy.array(image_1,numpy.int16)
    image2 = numpy.array(image_2,numpy.int16)
    image_diff = numpy.sum(numpy.abs(image1-image2))
    return (image_diff / 255.0 * 100) / image1.size



## ----------------------------------- Mutation Functions ---------------------------------- ##

def mutate_vertice(position):
    # Function to modify the vertice of a polygon during mutation.
    mut_idx = random.choice(range(len(position)))
    temp_list = list(position[mut_idx])
    x = (temp_list[0] + random.randint(-10,10))%100 # to control the amount of change from the old point
    y =(temp_list[1] + random.randint(-10,10))%100
    position[mut_idx] = (x,y)
    return position

def mutate_child(child):
    """
    Function to call mutation on a single image and return the result together with the new fitness
    """
    image = deepcopy(child)
    image.mutate()
    image_1 = image.drawImage()
    target = TARGET
    return (image, fitness(image_1,target))

def test_mutation(child,POP_TEST_SIZE):

    #Mutate child several times based test population size
    results = [mutate_child(child) for child in [child]*int(POP_TEST_SIZE)]
    return results
        


## ---------------------------------- CROSSOVER FUNCTION --------------------------------------##
"""
Funtion to perform crossover based on crossover rate, to produce new child from parents.
Crossover point is determined randomly but limited to a few polygons in order not to  change 
the image so much.
Two children generated but only the best is kept.
"""

def crossover(parent_1,parent_1_fitness, parent_2, parent_2_fitness, crossover_rate):
    if crossover_rate > random.random():
        
        # Define crossover points relative to the number of polygons(max is 10% of number of polygons)
        crossover_pt = random.randint(0, len(parent_1.polygons)//10)
        temp = parent_1.polygons[:crossover_pt]
        
        #perform swap of polygons on parents
        parent_1.polygons[:crossover_pt] = parent_2.polygons[:crossover_pt]
        parent_2.polygons[:crossover_pt] = temp
        p1_fitness = fitness(parent_1.drawImage(), TARGET)
        p2_fitness = fitness(parent_2.drawImage(), TARGET)    
        
        # check and keep only the best results
        if p1_fitness < p2_fitness:
            return (parent_1, p1_fitness)
        else:
            return (parent_2, p2_fitness)
    else:
        return(parent_2, parent_1_fitness)



## ---------------------------------- MAIN FUNCTION --------------------------------------## 

def main(TARGET=TARGET):
    
    # create new directory to store the best solution in generations
    if not os.path.exists("IMAGES"):
        os.mkdir("IMAGES")
        
    #Create results file
    f = open(os.path.join("IMAGES","results.txt"),'a')
    
    # Generate initial population based on population size and number of polygons per image
    # Population is sorted based on the fitness so the best is always the first
    population = [Image(TARGET.size, POLYGONS_NUMBER) for _ in range(POPULATION_SIZE)]
    population = [(image ,fitness(image.drawImage(), TARGET)) for image in population]
    population = sorted(population, key=lambda x: x[1])
    
    #Initialize generation at 1
    generation = 1
    
    # loop created to be endless unless interrupted
    while True:
        # select parents for crossover, top 2 parents in population are selected
        parent_1, parent_1_fitness = population[0]
        parent_2, parent_2_fitness = population[1]
        
        #create new children population
        children = []
        
        # top parent gets added to new children population automatically to ensure we have 
        # as a minimum the fitness from the previous generation
        children.append((parent_1, parent_1_fitness))
        
        #produce new child image by recombining both parents
        new_child, new_child_fitness = crossover(parent_1, parent_1_fitness, parent_2,parent_2_fitness, CROSSOVER_RATE)
        
        # new child is only accepted into population if fitness is better than that of
        # of the second parent
        if new_child_fitness < parent_2_fitness:
            children.append((new_child,new_child_fitness))
        else:
            children.append((parent_2, parent_2_fitness))
            
        # Mutate a number of times both individuals in children population
        # 2 separate mutation lists from both children are merged into one
        results = [test_mutation(child[0],POP_TEST_SIZE-1) for child in children]
        results = [item for sublist in results for item in sublist] 
        
        # add all the results to the children population and sort
        children.extend(results)
        children = sorted(children, key=lambda x : x[1])
        
        # create new population by keep only top 2 
        population = children[:POPULATION_SIZE]
        
        best  = population[0]
        best_image = best[0]
        best_fitness = best[1]
        
        #Print the best fitness for each generation and write it to the results file
        print ("Generation {} - {}".format(generation, best_fitness))
        f.write("Generation {} - {}\n".format(generation, best_fitness))

        #Save an image of the current best organism to the results directory
        if (generation) % GENERATIONS_TO_SAVE == 0:
            best_image.drawImage().save(os.path.join("IMAGES","{}.png".format(generation)))
        
        #go to next generation
        generation += 1    
           

if __name__ == "__main__":
    main()



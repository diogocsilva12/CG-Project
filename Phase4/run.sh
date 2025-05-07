#!/bin/bash

# Function to generate a temporary XML file for viewing a single figure
generate_view_xml() {
    local filename=$1
    local xml_path="engine/configs/temp_view.xml"
    
    # Extract figure type from filename for better camera positioning
    local figure_type=$(echo "$filename" | cut -d'_' -f1)
    
    # Set default camera position
    local camera_x=3
    local camera_y=3
    local camera_z=3
    local far_plane=100
    
    # Adjust camera based on figure type
    case $figure_type in
        box)
            camera_x=5
            camera_y=5
            camera_z=5
            ;;
        sphere)
            camera_x=3
            camera_y=3
            camera_z=3
            ;;
        torus)
            camera_x=4
            camera_y=4
            camera_z=4
            ;;
        cone)
            camera_x=4
            camera_y=6
            camera_z=4
            ;;
        plane)
            camera_x=3
            camera_y=5
            camera_z=3
            ;;
        *)
            camera_x=5
            camera_y=5
            camera_z=5
            ;;
    esac
    
    # Create the XML file
    cat > "$xml_path" << EOF
<world>
    <window width="800" height="600" />
    <camera>
        <position x="$camera_x" y="$camera_y" z="$camera_z" />
        <lookAt x="0" y="0" z="0" />
        <up x="0" y="1" z="0" />
        <projection fov="60" near="0.1" far="$far_plane" />
    </camera>

    <group>
        <transform>
            <!-- Center the figure -->
        </transform>
        <models>
            <model file="$filename" />
        </models>
    </group>
    
    <!-- Add coordinate axes for reference -->
    <group>
        <models>
            <model file="axis.3d" />
        </models>
    </group>
</world>
EOF
    
    echo "$xml_path"
}

# Function to generate axis model file if it doesn't exist
generate_axis_file() {
    local axis_file="tests/axis.3d"
    
    # Only create if it doesn't exist
    if [ ! -f "$axis_file" ]; then
        echo "Generating axis reference file..."
        
        # Create a simple axis model (x=red, y=green, z=blue)
        cat > "$axis_file" << EOF
# X axis (length 2)
0 0 0
2 0 0

# Y axis (length 2)
0 0 0
0 2 0

# Z axis (length 2)
0 0 0
0 0 2
EOF
    fi
}

# Function to view figure menu with persistent viewing
view_figures_menu() {
    local return_to_main=false
    
    while [ "$return_to_main" = false ]; do
        clear
        echo "=== View Generated Figures ==="
        echo "Available figures:"
        
        if [ -d "tests" ]; then
            # Ensure we have an axis file for reference
            generate_axis_file
            
            # Get all 3D models in the tests directory
            # Exclude the axis.3d file from the list
            files=(tests/*.3d)
            valid_files=()
            counter=1
            
            if [ ${#files[@]} -eq 0 ] || [ ! -e "${files[0]}" ]; then
                echo "No generated figures found."
                read -p "Press Enter to return to the main menu..."
                return_to_main=true
            else
                # Display all files with numbers
                for file in "${files[@]}"; do
                    filename=$(basename "$file")
                    if [ "$filename" != "axis.3d" ]; then
                        echo "$counter) ${filename}"
                        valid_files[$counter]=$filename
                        ((counter++))
                    fi
                done
                
                echo "0. Return to main menu"
                read -p "Select figure number to view (0 to return): " figure_number
                
                case $figure_number in
                    0)
                        return_to_main=true
                        ;;
                    *)
                        if [ "$figure_number" -ge 1 ] && [ "$figure_number" -lt "$counter" ]; then
                            selected_file="${valid_files[$figure_number]}"
                            
                            # Generate a temporary XML file for this figure
                            xml_path=$(generate_view_xml "$selected_file")
                            
                            echo "Viewing figure: $selected_file"
                            echo "(Close the viewer window when done to return to this menu)"
                            
                            # Run the engine with the generated XML
                            cd build
                            ./engine "../$xml_path"
                            cd ..
                            
                            # Ask if the user wants to view another figure or return to main menu
                            echo ""
                            read -p "Press Enter to continue viewing figures..."
                        else
                            echo "Invalid figure number!"
                            read -p "Press Enter to try again..."
                        fi
                        ;;
                esac
            fi
        else
            echo "No figures directory found!"
            mkdir -p tests
            echo "Created tests directory. Generate some figures first."
            read -p "Press Enter to return to the main menu..."
            return_to_main=true
        fi
    done
}

# Function to run test files with persistent viewing
run_tests_menu() {
    local return_to_main=false
    
    while [ "$return_to_main" = false ]; do
        clear
        echo "=== Available Test Files ==="
        if [ -d "engine/configs" ]; then
            # Only show files that start with "test" and the solar system
            files=(engine/configs/test*.xml engine/configs/static_solar_system.xml engine/configs/dynamic_solar_system.xml)
            if [ ${#files[@]} -eq 0 ] || [ ! -e "${files[0]}" ]; then
                echo "No test files found."
                read -p "Press Enter to return to the main menu..."
                return_to_main=true
            else
                echo "Test configurations:"
                # Create an array to store the filenames
                declare -a test_files
                counter=1
                
                # List files with numbers
                for file in "${files[@]}"; do
                    if [ -f "$file" ]; then
                        filename=$(basename "$file" .xml)
                        echo "$counter) ${filename}"
                        test_files[$counter]=$filename
                        ((counter++))
                    fi
                done
                
                echo "0. Return to main menu"
                read -p "Select test number (0 to return): " test_number
                
                case $test_number in
                    0)
                        return_to_main=true
                        ;;
                    *)
                        if [ "$test_number" -ge 1 ] && [ "$test_number" -lt "$counter" ]; then
                            selected_test="${test_files[$test_number]}"
                            if [ -f "engine/configs/${selected_test}.xml" ]; then
                                echo "Running test: $selected_test"
                                echo "(Close the viewer window when done to return to this menu)"
                                cd build
                                ./engine "../engine/configs/${selected_test}.xml"
                                cd ..
                                
                                # Ask if the user wants to run another test or return to main menu
                                echo ""
                                read -p "Press Enter to continue viewing tests..."
                            fi
                        else
                            echo "Invalid test number!"
                            read -p "Press Enter to try again..."
                        fi
                        ;;
                esac
            fi
        else
            echo "No test configs directory found!"
            read -p "Press Enter to return to the main menu..."
            return_to_main=true
        fi
    done
}

# Function to run phase 4 test files with persistent viewing
run_phase4_tests_menu() {
    local return_to_main=false
    local test_dir="test files/test_files_phase_4"
    while [ "$return_to_main" = false ]; do
        clear
        echo "=== Phase 4 Test Files ==="
        if [ -d "$test_dir" ]; then
            files=("$test_dir"/*.xml)
            if [ ${#files[@]} -eq 0 ] || [ ! -e "${files[0]}" ]; then
                echo "No phase 4 test files found."
                read -p "Press Enter to return to the main menu..."
                return_to_main=true
            else
                echo "Test files:"
                declare -a test_files
                counter=1
                for file in "${files[@]}"; do
                    if [ -f "$file" ]; then
                        filename=$(basename "$file" .xml)
                        echo "$counter) $filename"
                        test_files[$counter]=$filename
                        ((counter++))
                    fi
                done
                echo "0. Return to main menu"
                read -p "Select test number (0 to return): " test_number
                case $test_number in
                    0)
                        return_to_main=true
                        ;;
                    *)
                        if [ "$test_number" -ge 1 ] && [ "$test_number" -lt "$counter" ]; then
                            selected_test="${test_files[$test_number]}"
                            if [ -f "$test_dir/${selected_test}.xml" ]; then
                                echo "Running test: $selected_test"
                                echo "(Close the viewer window when done to return to this menu)"
                                cd build
                                ./engine "../$test_dir/${selected_test}.xml"
                                cd ..
                                echo ""
                                read -p "Press Enter to continue viewing tests..."
                            fi
                        else
                            echo "Invalid test number!"
                            read -p "Press Enter to try again..."
                        fi
                        ;;
                esac
            fi
        else
            echo "No phase 4 test files directory found!"
            read -p "Press Enter to return to the main menu..."
            return_to_main=true
        fi
    done
}

# Main menu
while true; do
    clear
    echo "====== 3D Figure Generator and Viewer ======"
    echo "1. Clean and Build"
    echo "2. Generate Figure"
    echo "3. View Generated Figures"
    echo "4. Run Test Files"
    echo "5. Run Phase 4 Test Files"
    echo "0. Exit"
    echo "============================================="
    
    read -p "Choose an option: " option

    case $option in
        1)
            echo "Cleaning and building..."
            if [ -d "build" ]; then
                cd build
                rm -rf *
            else
                mkdir build
                cd build
            fi
            cmake ..
            make
            cd ..
            read -p "Press Enter to return to the main menu..."
            ;;
            
        2)
            echo "=== Generate Figure ==="
            echo "1. Plane"
            echo "2. Box"
            echo "3. Sphere"
            echo "4. Cone"
            echo "5. Torus"
            echo "6. Bezier Patch"
            echo "0. Back"
                        
            read -p "Select figure type: " figure
            
            case $figure in
            1)
                echo "=== Plane Parameters ==="
                echo "Example: unit=2, slices=4"
                echo "Default: unit=1, slices=3"
                read -p "Enter unit size (default=1): " unit
                read -p "Enter number of slices (default=3): " slices
                unit=${unit:-1}
                slices=${slices:-3}
                # Create standardized filename
                filename="plane_${unit}_${slices}.3d"
                # Ensure tests directory exists
                mkdir -p tests
                cd generator
                ./generator plane $unit $slices "../tests/$filename"
                cd ..
                echo "Generated: tests/$filename"
                read -p "Press Enter to return to the main menu..."
                ;;
            2)
                echo "=== Box Parameters ==="
                echo "Example: unit=2, slices=4"
                echo "Default: unit=2, slices=3"
                read -p "Enter unit size (default=2): " unit
                read -p "Enter number of slices (default=3): " slices
                unit=${unit:-2}
                slices=${slices:-3}
                # Create standardized filename
                filename="box_${unit}_${slices}.3d"
                # Ensure tests directory exists
                mkdir -p tests
                cd generator
                ./generator box $unit $slices "../tests/$filename"
                cd ..
                echo "Generated: tests/$filename"
                read -p "Press Enter to return to the main menu..."
                ;;
            3)
                echo "=== Sphere Parameters ==="
                echo "Example: radius=2, slices=10, stacks=10"
                echo "Default: radius=1, slices=10, stacks=10"
                read -p "Enter radius (default=1): " radius
                read -p "Enter slices (default=10): " slices
                read -p "Enter stacks (default=10): " stacks
                radius=${radius:-1}
                slices=${slices:-10}
                stacks=${stacks:-10}
                # Create standardized filename
                filename="sphere_${radius}_${slices}_${stacks}.3d"
                # Ensure tests directory exists
                mkdir -p tests
                cd generator
                ./generator sphere $radius $slices $stacks "../tests/$filename"
                cd ..
                echo "Generated: tests/$filename"
                read -p "Press Enter to return to the main menu..."
                ;;
            4)
                echo "=== Cone Parameters ==="
                echo "Example: radius=2, height=4, slices=10, stacks=10"
                echo "Default: radius=1, height=2, slices=4, stacks=3"
                read -p "Enter radius (default=1): " radius
                read -p "Enter height (default=2): " height
                read -p "Enter slices (default=4): " slices
                read -p "Enter stacks (default=3): " stacks
                radius=${radius:-1}
                height=${height:-2}
                slices=${slices:-4}
                stacks=${stacks:-3}
                # Create standardized filename
                filename="cone_${radius}_${height}_${slices}_${stacks}.3d"
                # Ensure tests directory exists
                mkdir -p tests
                cd generator
                ./generator cone $radius $height $slices $stacks "../tests/$filename"
                cd ..
                echo "Generated: tests/$filename"
                read -p "Press Enter to return to the main menu..."
                ;;
            
            5)
                echo "=== Torus Parameters ==="
                echo "Example: outer_radius=1, inner_radius=0.5, slices=16, stacks=8"
                echo "Default: outer_radius=1, inner_radius=0.5, slices=16, stacks=8"
                read -p "Enter outer radius (default=1): " outer_radius
                read -p "Enter inner radius (default=0.5): " inner_radius
                read -p "Enter slices (default=16): " slices
                read -p "Enter stacks (default=8): " stacks
                outer_radius=${outer_radius:-1}
                inner_radius=${inner_radius:-0.5}
                slices=${slices:-16}
                stacks=${stacks:-8}
                # Create standardized filename
                filename="torus_${outer_radius}_${inner_radius}_${slices}_${stacks}.3d"
                # Ensure tests directory exists
                mkdir -p tests
                cd generator
                ./generator torus $outer_radius $inner_radius $slices $stacks "../tests/$filename"
                cd ..
                echo "Generated: tests/$filename"
                read -p "Press Enter to return to the main menu..."
                ;;

            6)
                echo "=== Bezier Patch Parameters ==="
                echo "Example: patch_file=teapot.patch, tessellation=20"
                echo "Default: patch_file=teapot.patch, tessellation=10"
                
                # Check if we have patch files available
                if [ ! -d "patches" ]; then
                    echo "Creating patches directory..."
                    mkdir -p patches
                    echo "Error: No patch files found. Please add .patch files to the 'patches' directory."
                    read -p "Press Enter to return to the main menu..."
                    continue
                fi
                
                # List available patch files
                echo "Available patch files:"
                patch_files=(patches/*.patch)
                if [ ${#patch_files[@]} -eq 0 ] || [ ! -e "${patch_files[0]}" ]; then
                    echo "No patch files found. Please add .patch files to the 'patches' directory."
                    read -p "Press Enter to return to the main menu..."
                    continue
                fi
                
                counter=1
                for file in "${patch_files[@]}"; do
                    filename=$(basename "$file")
                    echo "$counter) $filename"
                    available_patches[$counter]=$file
                    ((counter++))
                done
                
                read -p "Select patch file number (default=1): " patch_number
                patch_number=${patch_number:-1}
                
                if [ "$patch_number" -ge 1 ] && [ "$patch_number" -lt "$counter" ]; then
                    selected_patch="${available_patches[$patch_number]}"
                    
                    read -p "Enter tessellation level (default=10): " tessellation
                    tessellation=${tessellation:-10}
                    
                    # Extract basename for filename
                    patch_basename=$(basename "$selected_patch" .patch)
                    
                    # Create standardized filename
                    filename="bezier_${patch_basename}_${tessellation}.3d"
                    
                    # Ensure tests directory exists
                    mkdir -p tests
                    cd generator
                    ./generator bezier "../${selected_patch}" $tessellation "../tests/$filename"
                    cd ..
                    echo "Generated: tests/$filename"
                else
                    echo "Invalid patch file selection!"
                fi
                read -p "Press Enter to return to the main menu..."
                ;;
            0)
                # Do nothing, just return to main menu
                ;;
            esac
            ;;
            
        3)
            # Call the figure viewing menu function
            view_figures_menu
            ;;

        4)
            # Call the test running menu function
            run_tests_menu
            ;;
            
        5)
            # Call the phase 4 test running menu function
            run_phase4_tests_menu
            ;;
            
        0)
            echo "Exiting..."
            exit 0
            ;;
            
        *)
            echo "Invalid option"
            read -p "Press Enter to try again..."
            ;;
    esac
done
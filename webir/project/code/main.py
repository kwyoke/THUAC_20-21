import streamlit as st
import searchquery
import index
import real_time_ocr
from searchquery import search
from index import createSearchableData
from real_time_ocr import main_ocr
from PIL import Image
import cv2
import SessionState

session = SessionState.get(run_id=0)

# Text/Title
st.title("Lecture video info retrieval GUI")
st.info("An application to quickly locate the relevant timestamp of a ppt based lecture video.")
st.text("Press 'Preprocess' button to perform OCR and indexing on unprocessed video")
st.text("Press 'Ready' button to show top N relevant timestamps and watch video.")
st.text("Press 'Reset' to refresh and enter new query.")

# Receiving User Text Input (vid filepath)
vid_dir = st.text_input("Enter video file path","0510_week12.mp4")

# Receiving User Text Input (search query)
query = st.text_input("Enter search query","stream processing")

# Receiving User Text Input (top N)
topN = st.text_input("Top N results","3")

indexdir = vid_dir[:-4] + "_index"

if st.button("Preprocess"):
    st.info("Perform OCR and indexing...")
    main_ocr(vid_dir)
    createSearchableData(vid_dir[:-4], indexdir)
    st.success("Preprocessing successful")
    st.image(vid_dir[:-4] + "/0_text.jpg",width=300,caption="")

if st.button("Reset"):
    session.run_id += 1

start_time = 0
# Ready button
if st.button("Ready"):
    st.write("Video path: ",vid_dir)
    st.write("Query: ",query)
    st.write("N: ",topN)
    topN = int(topN)

    #Get video fps
    video = cv2.VideoCapture(vid_dir)
    fps = video.get(cv2.CAP_PROP_FPS)

    #Get topN ranked results    
    top_outputs = search(indexdir, query, topN)

    # Display top frames on sidebar
    st.sidebar.header("Top frame results")

    for i in range(topN):
        img = Image.open(vid_dir[:-4] + "/" + top_outputs['imgpath'][i])
        capt = "timestamp (min): " + str(round(top_outputs['framenum'][i]/fps/60, 2)) + ", score: " + str(round(top_outputs['score'][i],2))
        
        st.sidebar.subheader("#" + str(i+1))
        st.sidebar.image(img,width=300,caption=capt)
        st.sidebar.text(top_outputs['slidetxt'][i])

    
    # Insert video with correct starting time
    vid_file = open(vid_dir,"rb").read()
    start_time = int(top_outputs['framenum'][0]/fps)
    st.video(vid_file, start_time=start_time)
import pandas as pd
import matplotlib.pyplot as plt

# Data extracted directly from the uploaded handwritten note
# Structure: [Algorithm, Scenario, Step1, Step2, Step3, Step4, Step5]
data = [
    # HQC
    ['HQC', 1, 9.1, 5541, 11177, 1620, 22.7],
    ['HQC', 2, 5587, 614, 17.0, 14062, 15350],
    # Kyber
    ['Kyber', 1, 1.9, 2501, 150, 176, 0.1],
    ['Kyber', 2, 100, 176, 1.8, 2687, 464],
    # Saber
    ['Saber', 1, 2.0, 2466, 194, 165, 0.1],
    ['Saber', 2, 113, 147, 1.5, 1913, 725]
]

# Create DataFrame
columns = ['Algorithm', 'Scenario', 'Step 1', 'Step 2', 'Step 3', 'Step 4', 'Step 5']
df = pd.DataFrame(data, columns=columns)
df['Label'] = df['Algorithm'] + ' - ' + df['Scenario'].astype(str)

# Operation Labels for the two scenarios
# Scenario 1 (PC Init): Calc PC -> Send PC -> Calc PSoC -> Send PSoC -> Calc PC
labels_1 = ['Cálculo (PC)', 'Envío (PC)', 'Cálculo (PSoC)', 'Envío (PSoC)', 'Cálculo (PC)']
# Scenario 2 (PSoC Init): Calc PSoC -> Send PSoC -> Calc PC -> Send PC -> Calc PSoC
labels_2 = ['Cálculo (PSoC)', 'Envío (PSoC)', 'Cálculo (PC)', 'Envío (PC)', 'Cálculo (PSoC)']

# Plotting
fig, ax = plt.subplots(figsize=(12, 6))

colors = {
    'Cálculo (PC)': '#4A90E2',   # Blue
    'Envío (PC)': '#F5A623',       # Orange
    'Cálculo (PSoC)': '#7ED321', # Green
    'Envío (PSoC)': '#D0021B'      # Red
}

for idx, row in df.iterrows():
    y = idx
    times = row[2:7].values
    current_labels = labels_1 if row['Scenario'] == 1 else labels_2
    
    current_x = 0
    for i, duration in enumerate(times):
        lbl = current_labels[i]
        c = colors[lbl]
        
        ax.barh(y, duration, left=current_x, height=0.6, color=c, edgecolor='black')
        
        # Label text
        if duration > 100:
            ax.text(current_x + duration/2, y, f"{duration}", 
                   ha='center', va='center', color='white', fontsize=8, fontweight='bold')
        current_x += duration

ax.set_yticks(range(len(df)))
ax.set_yticklabels(df['Label'], fontweight='bold')
ax.set_xlabel('Tiempo (ms)')
ax.set_title('Ánalisis de temporización del protocolo de comunicaciones.')

# Legend
from matplotlib.patches import Patch
legend_elements = [Patch(facecolor=colors[l], label=l) for l in colors]
ax.legend(handles=legend_elements, loc='upper right')

plt.tight_layout()
plt.show()
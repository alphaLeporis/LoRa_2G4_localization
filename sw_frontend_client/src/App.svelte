<script>
	let factor = 15.5244
	const pixelMRatio = 78.32/1215.87
	import { supabase } from './supabaseClient'
	
	
	async function getLastLocation() {
		let { data, error, status } = await supabase
        .from('distance_reading')
        .select(`location`)
        .eq('node', 1)
		.limit(1)
		.order('created_at', { ascending: false })
		return data[0].location
	}
	async function getAnchors() {
		let { data, error, status } = await supabase
        .from('anchor')
        .select(`location, anchor_id`)
        .eq('is_online', true)	
		return data
	}

	let lastLocation = getLastLocation()
	let anchors = getAnchors()
	go();
	window.addEventListener('resize', go);

	function go(){
		factor = document.documentElement.clientWidth/5026 * 15.5244
	}

	const subscription = supabase
    .from('distance_reading')
    .on('INSERT', payload => {
		lastLocation = payload.new.location
    })
  .subscribe()

</script>

<main>
	{#await lastLocation then data}
	<div id="canvas">
		{#await anchors then anchorsData}
			{#each anchorsData as anchorData}
				<div id="circle" style="left:{anchorData.location.x*factor-25}px; top:{anchorData.location.y*factor-25}px">
				<h2>{anchorData.anchor_id}</h2>
				</div>
			{/each}
		{/await}
		<div id="circle" style="left:{data.x*factor-25}px; top:{data.y*factor-25}px"></div>
		<div id="absolute">
			<img src="map.jpg"/>
		</div>
		
	</div>
	{/await}
</main>

<style>
	main {
		text-align: center;
		padding: 1em;
		max-width: 240px;
		margin: 0 auto;
	}

	h1 {
		color: #ff3e00;
		text-transform: uppercase;
		font-size: 4em;
		font-weight: 100;
	}
	#circle {
		width: 50px;
		height: 50px;
		-webkit-border-radius: 25px;
		-moz-border-radius: 25px;
		border-radius: 25px;
		background: red;
		position: absolute;
		z-index: 5;
    }

	#canvas {
		position: absolute;
		width: 100%;
		height: auto;
		top: 0;
		left: 0;
	}

	#absolute {
		position: absolute;
		width: 100%;
		height: auto;
		top: 0;
		left: 0;
	}


	@media (min-width: 640px) {
		main {
			max-width: none;
		}
	}
</style>
